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
#include "lenlab_version.h"
#include "usb/exception.h"
#include <QDebug>

namespace protocol {

Manager::Manager(QObject *parent) : QObject(parent)
{

}

void
Manager::start()
{
    Q_ASSERT(board.isNull());

    startTimer(500);
}

void
Manager::on_error()
{
    qDebug() << "Manager::on_error";

    board->deleteLater();
}

void
Manager::on_init(const pMessage &reply)
{
    Q_UNUSED(reply);

    auto transaction = board->getName();
    connect(transaction.data(), &Transaction::succeeded, this, &Manager::on_name);
    connect(transaction.data(), &Transaction::failed, this, &Manager::on_error);
}

void
Manager::on_name(const pMessage &reply)
{
    Q_UNUSED(reply);

    auto transaction = board->getVersion();
    connect(transaction.data(), &Transaction::succeeded, this, &Manager::on_version);
    connect(transaction.data(), &Transaction::failed, this, &Manager::on_error);
}

void
Manager::on_version(const pMessage &reply)
{
    auto length = reply->getIntBufferLength();
    if (length == 3) {
        auto array = reply->getIntBuffer();

        auto major = array[0];
        auto minor = array[1];
        auto revision = array[2];

        qDebug() << "Board::on_getVersion" << major << minor << revision;

        if (major == MAJOR && minor == MINOR) {
            emit ready(board);
        }
        else {
            auto msg = QString("Ungültige Version %1.%2.%3. Lenlab erwartet mindestens %4.%5").arg(major).arg(minor).arg(revision).arg(MAJOR).arg(MINOR);
            emit error(msg);
            qDebug() << msg;
        }
    }
    else {
        qDebug() << "Board::on_getVersion";

        emit error("Das Lenlab Board antwortet mit einer ungültigen Version");
        qDebug() << "Das Lenlab Board antwortet mit einer ungültigen Version";
    }
}

void
Manager::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());

    if (!board.isNull()) return;

    try {
        auto device = bus.query(LENLAB_VID, LENLAB_PID);
        if (device) {
            board = new Board(device, this);
            auto transaction = board->init();
            connect(transaction.data(), &Transaction::succeeded, this, &Manager::on_init);
            connect(transaction.data(), &Transaction::failed, this, &Manager::on_error);
        }
        else {
            startTimer(500);
        }
    } catch (const usb::Exception &e) {
        qDebug() << e.getMsg();
        startTimer(3000);
    }

}

} // namespace protocol
