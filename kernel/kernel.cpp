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
    for (size_t i = 0; i < handlers.size(); i++)
    {
        if (a.hashes[i].empty())
            a.hashes[i] = handlers[i]->compute(a.img);
        if (b.hashes[i].empty())
            b.hashes[i] = handlers[i]->compute(b.img);
        if (handlers[i]->compare(a.hashes[i], b.hashes[i]))
            return true;
    }
    return false;
}

KeyFramesExtractor::KeyFramesExtractor() : key_frame_nums(std::make_unique<std::vector<size_t>>()),
    curr_frame_num(0), prev_hash(nullptr) {}

void KeyFramesExtractor::process_next(const cv::Mat &frame)
{
    std::unique_ptr<CombinedHash> curr_hash = std::make_unique<CombinedHash>(frame);
    if (prev_hash != nullptr && !hash_handler.eval_comparison(*curr_hash, *prev_hash))
        resolve_cluster();
    prev_hash = std::move(curr_hash);
    equal_frame_nums_cluster.push_back(curr_frame_num);
    curr_frame_num++;
}

void KeyFramesExtractor::finish_processing()
{
    resolve_cluster();
}

std::unique_ptr<std::vector<size_t>> KeyFramesExtractor::grab_key_frame_nums()
{
    std::unique_ptr<std::vector<size_t>> grabbed = std::move(key_frame_nums);
    key_frame_nums = std::make_unique<std::vector<size_t>>();
    return grabbed;
}

void KeyFramesExtractor::resolve_cluster()
{
    if (equal_frame_nums_cluster.empty())
        throw std::logic_error("Resolving cluster is forbidden: cluster is empty.");
    key_frame_nums->push_back(equal_frame_nums_cluster[equal_frame_nums_cluster.size() / 2]);
    equal_frame_nums_cluster.clear();
}
