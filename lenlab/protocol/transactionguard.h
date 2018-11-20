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

#ifndef TRANSACTIONGUARD_H
#define TRANSACTIONGUARD_H

#include "message.h"
#include "usb/device.h"
#include "usb/packet.h"
#include <QMetaObject>
#include <QObject>
#include <mutex>

namespace protocol {

class TransactionGuard : public QObject
{
    Q_OBJECT

    std::unique_lock<std::mutex> lock;
    QMetaObject::Connection connection;

public:
    TransactionGuard(const usb::pDevice &device, QObject *parent = nullptr);
    ~TransactionGuard();

signals:
    void reply(const pMessage &);

public slots:
    void on_reply_packet(const usb::pPacket &);
};

} // namespace protocol

#endif // TRANSACTIONGUARD_H
