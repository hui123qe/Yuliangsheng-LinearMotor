#include "mainwindow.h"
#include "tt_tools/ttools.h"

#include <QApplication>
#include <QDebug>
#include <QScreen>

int main(int argc, char *argv[])
{


    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    QApplication a(argc, argv);

    int defaultLoginState = 1;
    loginstate = &defaultLoginState;

    QScreen* screen = QGuiApplication::primaryScreen();

    qDebug() << "geometry:" << screen->geometry();
    qDebug() << "available:" << screen->availableGeometry();
    qDebug() << "logical DPI:" << screen->logicalDotsPerInch();
    qDebug() << "physical DPI:" << screen->physicalDotsPerInch();
    qDebug() << "devicePixelRatio:" << screen->devicePixelRatio();


    MainWindow mainWindow;
    mainWindow.show();
    return a.exec();
}
