/*

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

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
