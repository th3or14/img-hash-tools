#include "similar-images-finder.hpp"

#include <QApplication>

#include <cstdio>
#include <iostream>

int main(int argc, char *argv[])
{
    static const std::string log_filename = "log.txt";
    if (std::freopen(log_filename.c_str(), "w", stderr) == nullptr)
    {
        std::cout << "Failed to redirect stderr to '" << log_filename << "'.\n";
        return EXIT_FAILURE;
    }
    QApplication a(argc, argv);
    SimilarImagesFinder w;
    w.show();
    return a.exec();
}
