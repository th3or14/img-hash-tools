#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <memory>
#include <queue>

#include <opencv2/img_hash.hpp>
#include <opencv2/highgui.hpp>

class HashHandlerBase
{
public:
    virtual ~HashHandlerBase() = default;
    cv::Mat compute(const cv::Mat &img);
    bool compare(const cv::Mat &hash_a, const cv::Mat &hash_b) const;

private:
    const cv::Ptr<cv::img_hash::ImgHashBase> hash_algorithm;
    const std::function<bool(double)> thresholding_predicate;

protected:
    HashHandlerBase(const cv::Ptr<cv::img_hash::ImgHashBase> &hash_algorithm,
                    const std::function<bool(double)> &thresholding_predicate);
};

class AHashHandler : public HashHandlerBase
{
public:
    explicit AHashHandler(const std::function<bool(double)> &thresholding_predicate =
            [](double hashes_diff) -> bool
    {
        return hashes_diff <= 15;
    });
};

class PHashHandler : public HashHandlerBase
{
public:
    explicit PHashHandler(const std::function<bool(double)> &thresholding_predicate =
            [](double hashes_diff) -> bool
    {
        return hashes_diff <= 15;
    });
};

class CHashHandler : public HashHandlerBase
{
public:
    explicit CHashHandler(const std::function<bool(double)> &thresholding_predicate =
            [](double hashes_diff) -> bool
    {
        return hashes_diff <= 5.5;
    });
};

class RHashHandler : public HashHandlerBase
{
public:
    explicit RHashHandler(const std::function<bool(double)> &thresholding_predicate =
            [](double hashes_diff) -> bool
    {
        // yes, >= here
        return hashes_diff >= 0.708;
    });
};

class CombinedHash
{
public:
    explicit CombinedHash(const cv::Mat &img);

    friend class CombinedHashHandler;

private:
    cv::Mat img;
    std::vector<cv::Mat> hashes;
};

class CombinedHashHandler
{
public:
    CombinedHashHandler();
    bool eval_comparison(CombinedHash &a, CombinedHash &b);

private:
    std::vector<std::unique_ptr<HashHandlerBase>> handlers;
};

#endif // KERNEL_HPP
