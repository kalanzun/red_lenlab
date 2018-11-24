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

#include "transaction.h"
#include <QDebug>

namespace protocol {

Transaction::Transaction(QObject *parent) :
    QObject(parent)
{
    qDebug() << "transaction";
}

Transaction::~Transaction()
{
    qDebug() << "~transaction";
}

void Transaction::start(const usb::pDevice &device, const pMessage &command, int timeout)
{
    lock.reset(new TransactionLock(device));

    if (!lock->lock) throw std::exception();

    lock->conn = connect(device.data(), &usb::Device::reply,
                         this, &Transaction::on_reply);

    device->send(command->getPacket());

    startTimer(timeout);

    qDebug() << "start";
}

void
Transaction::on_reply(const usb::pPacket &packet)
{
    watchdog = true;

    auto message = pMessage::create(packet);

    qDebug() << "on_reply" << message->getReply() << message->isLast();

    if (message->getReply() == Error) {
        lock.reset(nullptr);
        emit failed();
        deleteLater();
    }
    else {
        if (message->isLast()) {
            lock.reset(nullptr);
            successfull = true;
        }

        replies.append(message);
        emit reply(message);

        if (successfull) {
            emit succeeded(message);
            deleteLater();
        }
    }
}

void
Transaction::timerEvent(QTimerEvent *event)
{
    if (successfull) {
        killTimer(event->timerId());
    }
    else {
        if (watchdog) {
            watchdog = false;
        }
        else {
            lock.reset(nullptr);
            killTimer(event->timerId());
            qDebug() << "transaction failed";
            emit failed();
            deleteLater();
        }
    }
}

} // namespace protocol
