#include "kernel.hpp"

namespace {

template <typename T>
bool get_thresholding_predicate(double hashes_diff);

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
std::unique_ptr<HashHandler> get_hash_handler()
{
    return std::make_unique<HashHandler>(T::create(), get_thresholding_predicate<T>);
}

}

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

CombinedHash::CombinedHash(const cv::Mat &img) : img(img) {}

CombinedHashHandler::CombinedHashHandler() :
    handlers{get_hash_handler<cv::img_hash::AverageHash>(),
             get_hash_handler<cv::img_hash::PHash>(),
             get_hash_handler<cv::img_hash::ColorMomentHash>(),
             get_hash_handler<cv::img_hash::RadialVarianceHash>()} {}

bool CombinedHashHandler::eval_comparison(CombinedHash &a, CombinedHash &b)
{
    if (a.img.empty() || b.img.empty())
        throw std::logic_error("Evaluating comparison is forbidden: empty image.");
    a.hashes.resize(handlers.size());
    b.hashes.resize(handlers.size());
    for (size_t i = 0; i < handlers.size(); ++i)
    {
        if (a.hashes.at(i).empty())
            a.hashes.at(i) = handlers.at(i)->compute(a.img);
        if (b.hashes.at(i).empty())
            b.hashes.at(i) = handlers.at(i)->compute(b.img);
        if (handlers.at(i)->compare(a.hashes.at(i), b.hashes.at(i)))
            return true;
    }
    return false;
}
