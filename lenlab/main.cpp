#include "model/lenlab.h"
#include "usb/handler.h"
#include "gui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    usb::Handler handler;

    model::Lenlab lenlab;
    lenlab.setHandler(&handler);

    /*
    QObject::connect(&handler, SIGNAL(reply(usb::pMessage)),
                     &lenlab, SIGNAL(reply(usb::pMessage)));

    QObject::connect(&lenlab, SIGNAL(command(usb::pMessage)),
                     &handler, SIGNAL(command(usb::pMessage)));
    */
    gui::MainWindow main_window;
    main_window.setModel(&lenlab);
    main_window.setHandler(&handler);
    main_window.show();

    return application.exec();
}
