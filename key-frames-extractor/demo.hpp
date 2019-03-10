#ifndef DEMO_HPP
#define DEMO_HPP

#include <kernel/kernel.hpp>

#include <iostream>

#include <QTime>
#include <QDir>

class DemoVideo
{
public:
    static void start(const std::string &input_video_filename, const std::string &output_location);

private:
    DemoVideo(const std::string &input_video_filename, const std::string &output_location);
    void locate_key_frames();
    void extract_key_frames();

    const std::string input_video_filename;
    const std::string output_location;
    const QString timestamp_format;
    const QString datetimestamp_format;
    const std::string key_frames_directory;
    cv::VideoCapture cap;
    std::unique_ptr<std::vector<size_t>> key_frame_nums;
};

#endif // DEMO_HPP
