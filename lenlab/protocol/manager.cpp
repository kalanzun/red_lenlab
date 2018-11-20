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

void Manager::on_board_ready()
{
    qDebug() << "on_board_ready";
    emit ready();
}

void Manager::on_board_error(const QString &msg)
{
    qDebug() << "on_board_error";
    emit error(msg);

    board.reset();
    startTimer(5000);
}

void
Manager::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());

    auto device = bus.query(LENLAB_VID, LENLAB_PID);

    if (device) {
        qDebug() << "create board";
        board = pBoard(new Board(device));//::create(device);
        qDebug() << "connect";
        connect(board.data(), &Board::ready, this, &Manager::on_board_ready);
        connect(board.data(), &Board::error, this, &Manager::on_board_error);
    }
    else {
        startTimer(500);
    }
}

} // namespace protocol
