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
    virtual ~HashHandler() = default;
    cv::Mat compute(const cv::Mat &img);
    bool compare(const cv::Mat &hash_a, const cv::Mat &hash_b) const;

private:
    const cv::Ptr<cv::img_hash::ImgHashBase> hash_algorithm;
    const std::function<bool(double)> thresholding_predicate;
};

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

#endif // KERNEL_HPP
