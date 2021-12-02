#include <QApplication>

#include "app/mainwindow.h"
#include "controller/lenlab.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    controller::Lenlab lenlab;
    app::MainWindow window;

    window.show();
    lenlab.lookForBoard();
    return application.exec();
}
