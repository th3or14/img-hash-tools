#include "kernel.hpp"

HashHandlerBase::HashHandlerBase(const cv::Ptr<cv::img_hash::ImgHashBase> &hash_algorithm,
                                 const std::function<bool(double)> &thresholding_predicate) :
    hash_algorithm(hash_algorithm), thresholding_predicate(thresholding_predicate) {}

cv::Mat HashHandlerBase::compute(const cv::Mat &img)
{
    cv::Mat hash;
    hash_algorithm->compute(img, hash);
    return hash;
}

bool HashHandlerBase::compare(const cv::Mat &hash_a, const cv::Mat &hash_b) const
{
    return thresholding_predicate(hash_algorithm->compare(hash_a, hash_b));
}

AHashHandler::AHashHandler(const std::function<bool(double)> &thresholding_predicate) :
    HashHandlerBase(cv::img_hash::AverageHash::create(), thresholding_predicate) {}

PHashHandler::PHashHandler(const std::function<bool(double)> &thresholding_predicate) :
    HashHandlerBase(cv::img_hash::PHash::create(), thresholding_predicate) {}

CHashHandler::CHashHandler(const std::function<bool(double)> &thresholding_predicate) :
    HashHandlerBase(cv::img_hash::ColorMomentHash::create(), thresholding_predicate) {}

RHashHandler::RHashHandler(const std::function<bool(double)> &thresholding_predicate) :
    HashHandlerBase(cv::img_hash::RadialVarianceHash::create(), thresholding_predicate) {}

CombinedHash::CombinedHash(const cv::Mat &img) : img(img) {}

CombinedHashHandler::CombinedHashHandler()
{
    handlers.push_back(std::make_unique<AHashHandler>());
    handlers.push_back(std::make_unique<PHashHandler>());
    handlers.push_back(std::make_unique<CHashHandler>());
    handlers.push_back(std::make_unique<RHashHandler>());
}

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
