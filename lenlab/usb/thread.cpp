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

#include "thread.h"
#include "libusb.h"
#include <QDebug>

using namespace usb;

Thread::Thread(QObject *parent) : QThread(parent)
{
    qDebug("thread");
}

Thread::~Thread()
{
    qDebug("~thread");
    wait();
}

void
Thread::run()
{
    qDebug("thread start");
    while(!isInterruptionRequested()) {
        libusb_handle_events(nullptr);
    }
    qDebug("thread stop");
}
