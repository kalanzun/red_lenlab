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

#include "manager.h"
#include "lenlab_protocol.h"
#include <QDebug>

namespace protocol {

Manager::Manager(QObject *parent) : QObject(parent)
{
    startTimer(500);

    connect(this, &Manager::send,
            this, &Manager::on_send,
            Qt::QueuedConnection);
}

void
Manager::on_send(const pTransaction &transaction, const pMessage &command, int timeout)
{
    connect(device.data(), &usb::Device::reply,
            transaction, &Transaction::on_reply_packet);
    transaction->startTimer(timeout);
    device->send(command->getPacket());
}

void
Manager::on_getName()
{
    qDebug() << "on_getName";
    auto cmd = pMessage::create();
    cmd->setCommand(getVersion);

    auto transaction = new Transaction();
    connect(transaction, &Transaction::succeeded,
            this, &Manager::on_getVersion);

    emit send(transaction, cmd, 100);
}

void
Manager::on_getVersion()
{
    qDebug() << "on_getVersion";

    emit ready();
}

void
Manager::query()
{
    device = bus.query(LENLAB_VID, LENLAB_PID);

    if (device) {
        auto cmd = pMessage::create();
        cmd->setCommand(getName);

        auto transaction = new Transaction();
        connect(transaction, &Transaction::succeeded,
                this, &Manager::on_getName);

        emit send(transaction, cmd, 100);
    }
    else {
        startTimer(500);
    }
}

void
Manager::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());

    query();
}

} // namespace protocol
