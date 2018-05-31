#include "demo.hpp"

void check_file_exists(const std::string &path)
{
    if (!QFile(QString::fromStdString(path)).exists())
        throw std::runtime_error("File '" + path + "' does not exist.");
}

void check_directory_exists(const std::string &path)
{
    if (!QDir(QString::fromStdString(path)).exists())
        throw std::runtime_error("Directory '" + path + "' does not exist.");
}

void try_create_directory(const std::string &path)
{
    if (!QDir().mkdir(QString::fromStdString(path)))
        throw std::runtime_error("Unable to create directory '" + path + "'.");
}

void try_open_video(cv::VideoCapture &vc, const std::string &path)
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
    key_frames_directory(output_location + "/" +
                         QDateTime::currentDateTime().toString(Qt::ISODateWithMs).toStdString()) {}

void DemoVideo::locate_key_frames()
{
    try_open_video(cap, input_video_filename);
    const size_t frames_cnt = cap.get(CV_CAP_PROP_FRAME_COUNT);
    std::cout << "Found " << frames_cnt << " frames to process.\n";
    KeyFramesExtractor extractor;
    PercentPrinter printer;
    for (size_t i = 0; i < frames_cnt; i++)
    {
        if (!cap.grab())
        {
            std::cout << "\nExtra break after frame " + std::to_string(i) + ". End of video?";
            break;
        }
        cv::Mat frame;
        cap.retrieve(frame);
        extractor.process_next(frame);
        printer.print_if_percent_changed(i + 1, frames_cnt,
                                         "\rLocating key frames (stage 1 of 2)... ", "%");
    }
    cap.release();
    extractor.finish_processing();
    key_frame_nums = extractor.grab_key_frame_nums();
    std::cout << "\nLocated " << key_frame_nums->size() << " key frames.\n";
}

void DemoVideo::extract_key_frames()
{
    if (key_frame_nums->empty())
        throw std::logic_error("Error: no key frames found.");
    try_open_video(cap, input_video_filename);
    std::vector<size_t>::const_iterator key_frame_num_it = key_frame_nums->begin();
    PercentPrinter printer;
    for (size_t curr_frame_num = 0; curr_frame_num < key_frame_nums->back() + 1; curr_frame_num++)
    {
        if (!cap.grab())
            throw std::logic_error("Error: reached end of video before end of extraction.");
        if (key_frame_num_it == key_frame_nums->end())
            throw std::logic_error("Error: nothing left to extract.");
        if (curr_frame_num == *key_frame_num_it)
        {
            cv::Mat curr_frame;
            cap.retrieve(curr_frame);
            cv::imwrite(key_frames_directory + "/" + QTime::fromMSecsSinceStartOfDay(
                            cap.get(CV_CAP_PROP_POS_MSEC)).toString(
                            Qt::ISODateWithMs).toStdString() + ".jpg", curr_frame);
            ++key_frame_num_it;
        }
        printer.print_if_percent_changed(curr_frame_num + 1, key_frame_nums->back() + 1,
                                         "\rExtracting key frames (stage 2 of 2)... ", "%");
    }
    cap.release();
    std::cout << "\n";
}

PercentPrinter::PercentPrinter() : displayed_percent(-1) {}

void PercentPrinter::print_if_percent_changed(const double current, const double total,
                                              const std::string &prefix,
                                              const std::string &postfix)
{
    const int actual_percent = current / total * 100;
    if (actual_percent != displayed_percent)
    {
        std::cout << prefix << actual_percent << postfix << std::flush;
        displayed_percent = actual_percent;
    }
}

void check_argc(const int argc_given, const int argc_expected)
{
    if (argc_given != argc_expected)
    {
        const std::string err_msg = "Wrong number of arguments (given " +
                std::to_string(argc_given - 1) + ", expected " +
                std::to_string(argc_expected - 1) + ").";
        throw std::runtime_error(err_msg);
    }
}
