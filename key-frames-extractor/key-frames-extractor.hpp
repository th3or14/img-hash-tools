#ifndef KEY_FRAMES_EXTRACTOR_HPP
#define KEY_FRAMES_EXTRACTOR_HPP

#include <hash-handler/hash-handler.hpp>

#include <iostream>

#include <QTime>
#include <QDir>

void extract_key_frames(const std::string &input_video_filename,
                        const std::string &output_location);

#endif // KEY_FRAMES_EXTRACTOR_HPP
