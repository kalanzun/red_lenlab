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

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "usb/device.h"
#include "usb/packet.h"
#include "message.h"
#include <QPointer>
#include <QTimerEvent>
#include <QObject>

namespace protocol {

class Transaction : public QObject
{
    Q_OBJECT

    bool watchdog = false;
    bool successfull = false;

public:
    explicit Transaction(QObject *parent = nullptr);
    ~Transaction();

signals:
    void reply(const pMessage &);

    void succeeded();
    void failed();

public slots:
    void on_reply_packet(const usb::pPacket &);

private:
    void timerEvent(QTimerEvent *);
};

typedef QPointer<Transaction> pTransaction;

} // namespace protocol

#endif // TRANSACTION_H
