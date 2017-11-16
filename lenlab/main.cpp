#include "model/lenlab.h"
#include "gui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    model::Lenlab lenlab;

    gui::MainWindow w(&lenlab);
    w.show();

    return a.exec();
}
