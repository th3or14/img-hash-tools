#ifndef DEMO_HPP
#define DEMO_HPP

#include <kernel/kernel.hpp>

#include <iostream>

#include <QTime>
#include <QDir>

void check_file_exists(const std::string &path);

void check_directory_exists(const std::string &path);

void try_create_directory(const std::string &path);

void try_open_video(cv::VideoCapture &vc, const std::string &path);

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
    const std::string key_frames_directory;
    cv::VideoCapture cap;
    std::unique_ptr<std::vector<size_t>> key_frame_nums;
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

void check_argc(int argc_given, int argc_expected);

#endif // DEMO_HPP
