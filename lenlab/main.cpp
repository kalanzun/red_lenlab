#include <QApplication>

#include "app/mainwindow.h"
#include "model/lenlab.h"
#include "usb/context.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    usb::Context context;
    model::Lenlab lenlab;
    app::MainWindow window;

    window.setModel(&lenlab);

    window.show();
    lenlab.lookForDevice();
    return application.exec();
}
