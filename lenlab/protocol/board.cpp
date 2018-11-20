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

namespace protocol {

Board::Board(usb::pDevice &device, QObject *parent) :
    QObject(parent),
    device(device)
{
    qDebug() << "board";
    getName();
}

void
Board::send(const pMessage &command)
{
    device->send(command->getPacket());
}

pTransaction
Board::call(const pMessage &command, int timeout)
{
    return new Transaction(device, command, timeout, this);
}

void
Board::getName()
{
    auto cmd = pMessage::create();
    cmd->setCommand(::getName);

    qDebug() << "call getName";

    auto transaction = call(cmd, 100);
    connect(transaction.data(), &Transaction::succeeded, this, &Board::on_getName);
}

void
Board::getVersion()
{
    auto cmd = pMessage::create();
    cmd->setCommand(::getVersion);

    qDebug() << "call getVersion";

    auto transaction = call(cmd, 100);
    connect(transaction.data(), &Transaction::succeeded, this, &Board::on_getVersion);
}

void
Board::on_getName(const pMessage &reply)
{
    name = reply->getString();

    auto prefix = QString("Lenlab red Firmware");
    if (name.startsWith(prefix)) {
        getVersion();
    }
    else {
        emit error("Das Lenlab Board antwortet mit einem unerwarteten Namen");
        qDebug() << "Das Lenlab Board antwortet mit einem unerwarteten Namen";
    }
}

void
Board::on_getVersion(const pMessage &reply)
{
    auto length = reply->getIntBufferLength();
    if (length == 3) {
        auto array = reply->getIntBuffer();

        major = array[0];
        minor = array[1];
        revision = array[2];

        if (major == MAJOR && minor == MINOR) {
            emit ready();
        }
        else {
            auto msg = QString("Ungültige Version %1.%2.%3. Lenlab erwartet mindestens %4.%5").arg(major).arg(minor).arg(revision).arg(MAJOR).arg(MINOR);
            emit error(msg);
            qDebug() << msg;
        }
    }
    else {
        emit error("Das Lenlab Board antwortet mit einer ungültigen Version");
        qDebug() << "Das Lenlab Board antwortet mit einer ungültigen Version";
    }
}

} // namespace protocol
