#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <memory>
#include <queue>

#include <opencv2/img_hash.hpp>
#include <opencv2/highgui.hpp>

class HashHandler
{
public:
    HashHandler(const cv::Ptr<cv::img_hash::ImgHashBase> &hash_algorithm,
                const std::function<bool(double)> &thresholding_predicate);
    cv::Mat compute(const cv::Mat &img);
    bool compare(const cv::Mat &hash_a, const cv::Mat &hash_b) const;

private:
    const cv::Ptr<cv::img_hash::ImgHashBase> hash_algorithm;
    const std::function<bool(double)> thresholding_predicate;
};

#endif // KERNEL_HPP
