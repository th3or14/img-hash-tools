#include "key-frames-extractor.hpp"

namespace {

struct CombinedHash
{
    cv::Mat img;
    std::vector<cv::Mat> hashes;

    explicit CombinedHash(const cv::Mat &img);
};

class CombinedHashHandler
{
public:
    CombinedHashHandler();
    bool eval_comparison(CombinedHash &a, CombinedHash &b);

private:
    std::array<std::unique_ptr<HashHandler>, 4> handlers;
};

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

class KeyFramesExtractor
{
public:
    void locate_key_frames(const QString &input_video_filename);
    void extract_key_frames(const QString &key_frames_directory);

private:
    cv::VideoCapture cap;
    std::vector<size_t> key_frame_nums;
};

} // namespace

static const QString timestamp_format("HH-mm-ss-zzz");
static const QString datetimestamp_format("yyyy-MM-ddT" + timestamp_format);

template <typename T>
static bool get_thresholding_predicate(double hashes_diff);

template<>
bool get_thresholding_predicate<cv::img_hash::AverageHash>(double hashes_diff)
{
    return hashes_diff <= 15;
}

template<>
bool get_thresholding_predicate<cv::img_hash::PHash>(double hashes_diff)
{
    return hashes_diff <= 15;
}

template<>
bool get_thresholding_predicate<cv::img_hash::ColorMomentHash>(double hashes_diff)
{
    return hashes_diff <= 5.5;
}

template<>
bool get_thresholding_predicate<cv::img_hash::RadialVarianceHash>(double hashes_diff)
{
    // yes, >= here
    return hashes_diff >= 0.708;
}

template <typename T>
static std::unique_ptr<HashHandler> get_hash_handler()
{
    return std::make_unique<HashHandler>(T::create(), get_thresholding_predicate<T>);
}

static void check_file_exists(const QString &path)
{
    if (!QFile(path).exists())
        throw std::runtime_error("File '" + path.toStdString() + "' does not exist.");
}

static void check_directory_exists(const QString &path)
{
    if (!QDir(path).exists())
        throw std::runtime_error("Directory '" + path.toStdString() + "' does not exist.");
}

static void try_create_directory(const QString &path)
{
    if (!QDir().mkdir(path))
        throw std::runtime_error("Unable to create directory '" + path.toStdString() + "'.");
}

static void try_open_video(cv::VideoCapture &vc, const QString &path)
{
    if (!vc.open(path.toStdString()))
        throw std::runtime_error("Unable to open '" + path.toStdString() +
                                 "'. Not a video or video format is not supproted.");
}

CombinedHash::CombinedHash(const cv::Mat &img) : img(img) {}

CombinedHashHandler::CombinedHashHandler() :
    handlers{get_hash_handler<cv::img_hash::AverageHash>(),
             get_hash_handler<cv::img_hash::PHash>(),
             get_hash_handler<cv::img_hash::ColorMomentHash>(),
             get_hash_handler<cv::img_hash::RadialVarianceHash>()} {}

bool CombinedHashHandler::eval_comparison(CombinedHash &a, CombinedHash &b)
{
    std::array<CombinedHash *, 2> a_and_b = {&a, &b};
    for (auto combined_hash : a_and_b)
        if (combined_hash->img.empty())
            throw std::logic_error("Evaluating comparison is forbidden: empty image.");
    for (auto combined_hash : a_and_b)
        combined_hash->hashes.resize(handlers.size());
    for (size_t i = 0; i < handlers.size(); ++i)
    {
        for (auto combined_hash : a_and_b)
            if (combined_hash->hashes.at(i).empty())
                combined_hash->hashes.at(i) = handlers.at(i)->compute(combined_hash->img);
        if (handlers.at(i)->compare(a.hashes.at(i), b.hashes.at(i)))
            return true;
    }
    return false;
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

void KeyFramesExtractor::locate_key_frames(const QString &input_video_filename)
{
    key_frame_nums.clear();
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
    for (size_t i = 1; i < borders.size(); ++i)
        // b + (a - b) / 2, a >= b (crucial for unsigned)
        // used against potential overflow of (a + b) / 2
        key_frame_nums.push_back(borders.at(i - 1) +
                                 (borders.at(i) -
                                  borders.at(i - 1)) / 2);
    std::cout << "\nLocated " << key_frame_nums.size() << " key frames.\n";
}

void KeyFramesExtractor::extract_key_frames(const QString &key_frames_directory)
{
    if (key_frame_nums.empty())
        throw std::logic_error("Error: no key frames found.");
    if (!cap.isOpened())
        throw std::logic_error("Error: video is not opened.");
    PercentPrinter printer;
    for (size_t i = 0; i < key_frame_nums.size(); ++i)
    {
        cap.set(cv::CAP_PROP_POS_FRAMES, key_frame_nums.at(i));
        if (!cap.grab())
            throw std::runtime_error("Error: reached end of video before end of extraction.");
        cv::Mat curr_frame;
        cap.retrieve(curr_frame);
        QString key_frame_filename = key_frames_directory + "/" + QTime::fromMSecsSinceStartOfDay(
                    cap.get(cv::CAP_PROP_POS_MSEC)).toString(timestamp_format) + ".jpg";
        cv::imwrite(key_frame_filename.toStdString(), curr_frame);
        printer.print_if_percent_changed(i + 1, key_frame_nums.size(),
                                         "\rExtracting key frames (stage 2 of 2)... ", "%");
    }
    std::cout << "\n";
}

void extract_key_frames(const QString &input_video_filename, const QString &output_location)
{
    check_file_exists(input_video_filename);
    check_directory_exists(output_location);
    QString key_frames_directory(output_location + "/" +
                                 QDateTime::currentDateTime().toString(datetimestamp_format));
    try_create_directory(key_frames_directory);
    KeyFramesExtractor kfe;
    kfe.locate_key_frames(input_video_filename);
    kfe.extract_key_frames(key_frames_directory);
}
