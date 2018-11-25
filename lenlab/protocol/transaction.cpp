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

static int p_transaction_type_id = qRegisterMetaType<pTransaction>("pTransaction");

Transaction::Transaction(const pMessage &command, QObject *parent) :
    QObject(parent),
    command(command)
{
    qDebug() << "transaction";
}

Transaction::~Transaction()
{
    qDebug() << "~transaction";
}

void
Transaction::setWatchdog(int timeout)
{
    this->timeout = timeout;
}

void
Transaction::startWatchdog()
{
    startTimer(timeout);
}

void
Transaction::on_reply(const usb::pPacket &packet)
{
    watchdog = true;

    auto message = pMessage::create(packet);

    qDebug() << "on_reply" << message->getReply() << message->isLast();

    if (message->getReply() == Error) {
        done = true;
        emit failed();
        emit finished();
    }
    else {
        replies.append(message);
        emit reply(message);

        if (message->isLast()) {
            done = true;
            emit succeeded(message);
            emit finished();
        }
    }
}

void
Transaction::timerEvent(QTimerEvent *event)
{
    if (done) {
        killTimer(event->timerId());
    }
    else {
        if (watchdog) {
            watchdog = false;
        }
        else {
            killTimer(event->timerId());
            qDebug() << "transaction failed";
            emit failed();
            emit finished();
        }
    }
}

} // namespace protocol
