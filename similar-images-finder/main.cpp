#include "widget.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    stderr = freopen("log.txt", "w", stderr);
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
