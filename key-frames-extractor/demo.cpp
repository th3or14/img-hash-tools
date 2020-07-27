#include "demo.hpp"

namespace {

class PercentPrinter
{
public:
    PercentPrinter();
    void print_if_percent_changed(double current, double total,
                                  const std::string &prefix = "",
                                  const std::string &postfix = "");

private:
    int displayed_percent;
};

} // namespace

static void check_file_exists(const std::string &path)
{
    if (!QFile(QString::fromStdString(path)).exists())
        throw std::runtime_error("File '" + path + "' does not exist.");
}

static void check_directory_exists(const std::string &path)
{
    if (!QDir(QString::fromStdString(path)).exists())
        throw std::runtime_error("Directory '" + path + "' does not exist.");
}

static void try_create_directory(const std::string &path)
{
    if (!QDir().mkdir(QString::fromStdString(path)))
        throw std::runtime_error("Unable to create directory '" + path + "'.");
}

static void try_open_video(cv::VideoCapture &vc, const std::string &path)
{
    if (!vc.open(path))
        throw std::runtime_error("Unable to open '" + path +
                                 "'. Not a video or video format is not supproted.");
}

void DemoVideo::start(const std::string &input_video_filename, const std::string &output_location)
{
    check_file_exists(input_video_filename);
    check_directory_exists(output_location);
    DemoVideo dv(input_video_filename, output_location);
    try_create_directory(dv.key_frames_directory);
    dv.locate_key_frames();
    dv.extract_key_frames();
}

DemoVideo::DemoVideo(const std::string &input_video_filename, const std::string &output_location) :
    input_video_filename(input_video_filename), output_location(output_location),
    timestamp_format("HH-mm-ss-zzz"), datetimestamp_format("yyyy-MM-ddT" + timestamp_format),
    key_frames_directory(output_location + "/" +
                         QDateTime::currentDateTime().toString(datetimestamp_format).toStdString())
{}

void DemoVideo::locate_key_frames()
{
    try_open_video(cap, input_video_filename);
    size_t frames_cnt = cap.get(cv::CAP_PROP_FRAME_COUNT);
    if (frames_cnt == 0)
        throw std::runtime_error("Found no frames to process.");
    std::cout << "Found " << frames_cnt << " frames to process.\n";
    PercentPrinter printer;
    CombinedHashHandler hash_handler;
    std::unique_ptr<CombinedHash> prev_hash = nullptr;
    std::vector<size_t> borders = {0};
    for (size_t i = 0; i < frames_cnt; ++i)
    {
        if (!cap.grab())
        {
            std::cout << "\nExtra break after frame " + std::to_string(i) + ". End of video?";
            break;
        }
        cv::Mat frame;
        cap.retrieve(frame);
        std::unique_ptr<CombinedHash> curr_hash = std::make_unique<CombinedHash>(frame);
        if ((prev_hash != nullptr) && !hash_handler.eval_comparison(*curr_hash, *prev_hash))
            borders.push_back(i);
        prev_hash = std::move(curr_hash);
        printer.print_if_percent_changed(i + 1, frames_cnt,
                                         "\rLocating key frames (stage 1 of 2)... ", "%");
    }
    borders.push_back(frames_cnt - 1);
    cap.release();
    for (size_t i = 1; i < borders.size(); ++i)
        // b + (a - b) / 2, a >= b (crucial for unsigned)
        // used against potential overflow of (a + b) / 2
        key_frame_nums.push_back(borders.at(i - 1) +
                                 (borders.at(i) -
                                  borders.at(i - 1)) / 2);
    std::cout << "\nLocated " << key_frame_nums.size() << " key frames.\n";
}

void DemoVideo::extract_key_frames()
{
    if (key_frame_nums.empty())
        throw std::logic_error("Error: no key frames found.");
    try_open_video(cap, input_video_filename);
    PercentPrinter printer;
    for (size_t i = 0; i < key_frame_nums.size(); ++i)
    {
        cap.set(cv::CAP_PROP_POS_FRAMES, key_frame_nums.at(i));
        if (!cap.grab())
            throw std::runtime_error("Error: reached end of video before end of extraction.");
        cv::Mat curr_frame;
        cap.retrieve(curr_frame);
        cv::imwrite(key_frames_directory + "/" + QTime::fromMSecsSinceStartOfDay(
                        cap.get(cv::CAP_PROP_POS_MSEC)).toString(
                        timestamp_format).toStdString() + ".jpg", curr_frame);
        printer.print_if_percent_changed(i + 1, key_frame_nums.size(),
                                         "\rExtracting key frames (stage 2 of 2)... ", "%");
    }
    cap.release();
    std::cout << "\n";
}

PercentPrinter::PercentPrinter() : displayed_percent(-1) {}

void PercentPrinter::print_if_percent_changed(double current, double total,
                                              const std::string &prefix,
                                              const std::string &postfix)
{
    int actual_percent = current / total * 100;
    if (actual_percent != displayed_percent)
    {
        std::cout << prefix << actual_percent << postfix << std::flush;
        displayed_percent = actual_percent;
    }
}
