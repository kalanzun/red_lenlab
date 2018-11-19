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

#ifndef DEVICE_H
#define DEVICE_H

#include "devhandle.h"
#include "eventloop.h"
#include "interface.h"
#include "packet.h"
#include "thread.h"
#include "transfer.h"
#include "libusb.h"
#include <QObject>
#include <QSharedPointer>
#include <memory>

namespace usb {

    class Device : public QObject
    {
        Q_OBJECT

        std::unique_ptr<Thread> thread;
        resource::DevHandle dev_handle;
        resource::EventLoop event_loop;
        resource::Interface interface;
        QSharedPointer<QVector<pPacket>> send_queue;
        std::unique_ptr<Transfer> sender;
        std::unique_ptr<Transfer> receiver0;
        std::unique_ptr<Transfer> receiver1;

    public:
        explicit Device(libusb_device *dev, QObject *parent = nullptr);

        void send(const pPacket &cmd);

    signals:
        void reply(const pPacket &);
        void error(const QString &);

    private slots:
        void on_reply(const pPacket &);
        void on_reply_transfer_ready();
        void on_send_transfer_ready();

    private:
        void try_to_send();

    };

    typedef QSharedPointer<Device> pDevice;

}

#endif // DEVICE_H
