#include "widget.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    redirect_stderr_to_log();
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
