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
}

pTransaction
Manager::call(const pMessage &command, int timeout)
{
    return new Transaction(device, command, timeout, this);
}

void
Manager::on_getName()
{
    qDebug() << "on_getName";
    auto cmd = pMessage::create();
    cmd->setCommand(getVersion);

    auto transaction = call(cmd, 100);
    connect(transaction.data(), &Transaction::succeeded,
            this, &Manager::on_getVersion);
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

        auto transaction = call(cmd, 100);
        connect(transaction.data(), &Transaction::succeeded,
                this, &Manager::on_getName);
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
