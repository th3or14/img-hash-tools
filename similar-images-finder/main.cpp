#include "similar-images-finder.hpp"

#include <QApplication>

#include <cstdio>
#include <iostream>

int main(int argc, char *argv[])
{
    static const char *log_filename = "log.txt";
    if (std::freopen(log_filename, "w", stderr) == nullptr)
    {
        std::cout << "Failed to redirect stderr to '" << log_filename << "'.\n";
        return EXIT_FAILURE;
    }
    QApplication a(argc, argv);
    SimilarImagesFinder sif;
    sif.show();
    return a.exec();
}
