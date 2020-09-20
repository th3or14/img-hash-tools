#ifndef KEY_FRAMES_EXTRACTOR_HPP
#define KEY_FRAMES_EXTRACTOR_HPP

#include <hash-handler/hash-handler.hpp>

#include <iostream>

#include <QTime>
#include <QDir>

#include <opencv2/imgproc.hpp>

void extract_key_frames(const QString &input_video_filename,
                        const QString &output_location);

#endif // KEY_FRAMES_EXTRACTOR_HPP
