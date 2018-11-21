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

#include "board.h"
#include "lenlab_protocol.h"
#include "lenlab_version.h"
#include <QDebug>
#include <QPointer>

namespace protocol {

Board::Board(usb::pDevice &device, QObject *parent) :
    QObject(parent),
    device(device)
{

}

void
Board::send(const pMessage &command)
{
    device->send(command->getPacket());
}

QPointer<Transaction>
Board::call(const pMessage &command, int timeout)
{
    std::unique_ptr<Transaction> transaction(new Transaction(this));

    transaction->start(device, command, timeout); // may throw

    return transaction.release();
}

QPointer<Transaction>
Board::init()
{
    qDebug() << "Board::init";

    auto cmd = pMessage::create();
    cmd->setCommand(::init);

    return call(cmd, 100);
}

QPointer<Transaction>
Board::getName()
{
    qDebug() << "Board::getName";

    auto cmd = pMessage::create();
    cmd->setCommand(::getName);

    return call(cmd, 100);
}

QPointer<Transaction>
Board::getVersion()
{
    qDebug() << "Board::getVersion";

    auto cmd = pMessage::create();
    cmd->setCommand(::getVersion);

    return call(cmd, 100);
}

} // namespace protocol
