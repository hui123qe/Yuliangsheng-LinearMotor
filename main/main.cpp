#include "widget.h"
#include "login.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    login w;

    w.show();
    //w.showMaximized();
    return a.exec();
}
