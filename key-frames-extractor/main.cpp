#include "key-frames-extractor.hpp"

static void check_argc(int argc_given, int argc_expected)
{
    if (argc_given != argc_expected)
    {
        std::string err_msg = "Wrong number of arguments (given " +
                std::to_string(argc_given - 1) + ", expected " +
                std::to_string(argc_expected - 1) + ").";
        throw std::runtime_error(err_msg);
    }
}

int main(int argc, char *argv[])
{
    try
    {
        check_argc(argc, 3);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Required arguments:\n\tinput video filename\n\toutput location\n";
        throw;
    }
    KeyFramesExtractor::start(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
