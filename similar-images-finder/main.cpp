#include "widget.hpp"

#include <QApplication>

#include <cstdio>

int main(int argc, char *argv[])
{
    stderr = std::freopen("log.txt", "w", stderr);
    QApplication a(argc, argv);
    SimilarImagesFinder w;
    w.show();
    return a.exec();
}
