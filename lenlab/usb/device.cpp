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

#include "device.h"
#include "exception.h"
#include <QDebug>

using namespace usb;

Device::Device(libusb_device *dev, QObject *parent) :
    QObject(parent),
    thread(new Thread()),
    dev_handle(dev),
    event_loop(thread.get()),
    interface(dev_handle.get()),
    send_queue(new QVector<pPacket>()),
    sender(new Transfer(dev_handle.get(), 0x01)),
    receiver0(new Transfer(dev_handle.get(), 0x81)),
    receiver1(new Transfer(dev_handle.get(), 0x81))
{
    connect(&*receiver0, SIGNAL(completed(pPacket)),
            this, SIGNAL(reply(pPacket)));
    connect(&*receiver1, SIGNAL(completed(pPacket)),
            this, SIGNAL(reply(pPacket)));
    connect(this, SIGNAL(reply(pPacket)),
            this, SLOT(on_reply(pPacket)));

    connect(&*sender, SIGNAL(completed(pPacket)),
            this, SLOT(on_send_transfer_ready()));
    connect(&*receiver0, SIGNAL(completed(pPacket)),
            this, SLOT(on_reply_transfer_ready()));
    connect(&*receiver1, SIGNAL(completed(pPacket)),
            this, SLOT(on_reply_transfer_ready()));

    connect(&*sender, SIGNAL(error(QString)),
            this, SIGNAL(error(QString)));
    connect(&*receiver0, SIGNAL(error(QString)),
            this, SIGNAL(error(QString)));
    connect(&*receiver1, SIGNAL(error(QString)),
            this, SIGNAL(error(QString)));

    receiver0->start(pPacket::create()); // may throw
    receiver1->start(pPacket::create()); // may throw

    thread->start();
}

void
Device::send(const pPacket &cmd)
{
    //qDebug() << "send" << cmd->getCommand();
    send_queue->append(cmd);
    try_to_send();
}

void
Device::on_reply(const pPacket &reply)
{
    Q_UNUSED(reply);
    //qDebug() << "reply" << reply->getReply() << reply->getPacketLength();
}

void
Device::on_reply_transfer_ready()
{
    try {
        qobject_cast<Transfer *>(QObject::sender())->start(pPacket::create());
    } catch (const Exception &e) {
        emit error(e.getMsg());
    }
}

void
Device::try_to_send()
{
    if (!send_queue->isEmpty() && !sender->isActive()) {
        try {
            sender->start(send_queue->takeFirst());
        } catch (const Exception &e) {
            emit error(e.getMsg());
        }
    }
}

void
Device::on_send_transfer_ready()
{
    try_to_send();
}
