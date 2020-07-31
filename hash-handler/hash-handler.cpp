#include "hash-handler.hpp"

HashHandler::HashHandler(const cv::Ptr<cv::img_hash::ImgHashBase> &hash_algorithm,
                         const std::function<bool(double)> &thresholding_predicate) :
    hash_algorithm(hash_algorithm), thresholding_predicate(thresholding_predicate) {}

cv::Mat HashHandler::compute(const cv::Mat &img)
{
    cv::Mat hash;
    hash_algorithm->compute(img, hash);
    return hash;
}

bool HashHandler::compare(const cv::Mat &hash_a, const cv::Mat &hash_b) const
{
    return thresholding_predicate(hash_algorithm->compare(hash_a, hash_b));
}
