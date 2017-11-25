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

#ifndef USBHANDLER_H
#define USBHANDLER_H

#include "context.h"
#include "message.h"
#include "device.h"
#include <QObject>
#include <QSharedPointer>

namespace usb {

    class Handler : public QObject
    {
        Q_OBJECT

        resource::Context ctx;
        QSharedPointer<Device> device;
        int timer_id;

    public:
        explicit Handler(QObject *parent = nullptr);

        void send(const pMessage &cmd);

        bool isReady();

    signals:
        void reply(const pMessage &);
        void ready();
        void error(const QString &);
        void logMessage(const QString &);

        void query();

    private slots:
        void on_reply(const pMessage &);
        void on_error(const QString &);
        void on_query();

    private:
        void open(libusb_device *dev);

        void timerEvent(QTimerEvent *event);
    };

}

#endif // USBHANDLER_H
