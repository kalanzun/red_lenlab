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

// TODO transaction blocking!!!

Transaction::Transaction(QObject *parent) : QObject(parent)
{
    qDebug() << "transaction";
    /*
    connect(device.data(), SIGNAL(reply(usb::pPacket)),
            this, SLOT(on_device_reply(usb::pPacket)));
    device->send(command->getPacket());
    startTimer(timeout);
    */
}

Transaction::~Transaction()
{
    qDebug() << "~transaction";
}

void
Transaction::on_reply_packet(const usb::pPacket &packet)
{
    watchdog = true;

    auto message = pMessage::create(packet);

    if (message->isLast()) {
        successfull = true;
    }

    emit reply(message);

    if (successfull) {
        emit succeeded();
        deleteLater();
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
            killTimer(event->timerId());
            emit failed();
            deleteLater();
        }
    }
}

} // namespace protocol
