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
#include <QMetaObject>
#include <QObject>
#include <QVector>

namespace protocol {

/*!
 * \brief The Board class
 *
 * Single instance only.
 *
 * Ownership: Child of Manager
 */
class Board : public QObject
{
    Q_OBJECT

    usb::pDevice device;
    pTransaction current_transaction;
    QMetaObject::Connection current_conn;
    QVector<pTransaction> queue;

public:
    explicit Board(usb::pDevice &device, QObject *parent = nullptr);

    void send(const pMessage &);
    void start(const pTransaction &);
    void send_queue();

    pTransaction init();
    pTransaction getName();
    pTransaction getVersion();
    pTransaction setSignalSine(uint32_t multiplier, uint32_t predivider, uint32_t divider, uint32_t amplitude, uint32_t second);
    pTransaction startOscilloscope(uint32_t samplerate);
    pTransaction startOscilloscopeLinearTestData();
    pTransaction startOscilloscopeTrigger(uint32_t samplerate);
    pTransaction startLogger(uint32_t interval);
    pTransaction stopLogger();

signals:
    void logger(const pMessage &);

private slots:
    void on_reply(const usb::pPacket &);
    void on_finished();
};

} // namespace protocol

#endif // BOARD_H
