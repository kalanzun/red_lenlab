#include "app/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    app::MainWindow w;
    w.show();
    return a.exec();
}
