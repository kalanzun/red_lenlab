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

#ifndef BOARD_H
#define BOARD_H

#include "message.h"
#include "transaction.h"
#include "usb/device.h"
#include <QObject>

namespace protocol {

class Board : public QObject
{
    Q_OBJECT

    usb::pDevice device;

public:
    explicit Board(usb::pDevice &device, QObject *parent = nullptr);

    void send(const pMessage &);

    QPointer<Transaction> call(const pMessage &, int);

    QPointer<Transaction> init();
    QPointer<Transaction> getName();
    QPointer<Transaction> getVersion();

signals:
};

} // namespace protocol

#endif // BOARD_H
