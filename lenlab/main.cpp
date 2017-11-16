#include "model/lenlab.h"
#include "usb/handler.h"
#include "gui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    model::Lenlab lenlab;

    usb::Handler handler;

    QObject::connect(&handler, SIGNAL(reply(usb::pMessage reply)),
                     &lenlab, SIGNAL(reply(usb::pMessage reply)));

    QObject::connect(&lenlab, SIGNAL(command(usb::pMessage command)),
                     &handler, SIGNAL(command(usb::pMessage command)));

    gui::MainWindow w(&lenlab);
    w.show();

    return a.exec();
}
