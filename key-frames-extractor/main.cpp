#include "key-frames-extractor.hpp"

int main(int argc, char *argv[])
{
    static const int expected_argc = 3;
    if (argc != expected_argc)
    {
        std::cout << "Wrong number of arguments (given " + std::to_string(argc - 1) +
                     ", expected " + std::to_string(expected_argc - 1) +
                     ").\nRequired arguments:\n\tinput video filename\n\toutput location\n";
        return EXIT_FAILURE;
    }
    extract_key_frames(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
