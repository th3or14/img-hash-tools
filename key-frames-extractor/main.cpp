#include "demo.hpp"

int main(int argc, char *argv[])
{
    try
    {
        check_argc(argc, 3);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Required arguments:\n\tinput video filename\n\toutput location\n";
        throw e;
    }
    DemoVideo::start(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
