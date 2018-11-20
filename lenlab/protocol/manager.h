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

#ifndef MANAGER_H
#define MANAGER_H

#include "board.h"
#include "usb/bus.h"
#include "usb/device.h"
#include <QTimerEvent>
#include <QObject>

namespace protocol {

class Manager : public QObject
{
    Q_OBJECT

    usb::Bus bus;
    pBoard board;

public:
    explicit Manager(QObject *parent = nullptr);

signals:
    void ready(const pBoard &);
    void error(const QString &);

public slots:
    void on_board_ready();
    void on_board_error(const QString &);

private:
    void timerEvent(QTimerEvent *);
};

typedef QSharedPointer<Manager> pManager;

} // namespace protocol

#endif // MANAGER_H
