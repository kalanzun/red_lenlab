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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "lenlab_protocol.h"
#include "usb/packet.h"
#include <QString>
#include <QVector>
#include <stdint.h>

#define MESSAGE_HEAD_LENGTH 1

namespace protocol {

/*!
 * \brief The Message class
 *
 * Ownership: QSharedPointer<Message>
 */
class Message
{
    usb::pPacket packet;

public:
    Message();
    Message(const usb::pPacket &);

    usb::pPacket &getPacket();

    size_t getHeadLength();
    uint8_t *getHead();

    void setCommand(Command);
    Command getCommand();

    void setReply(Reply reply);
    Reply getReply();

    void setType(Type type);
    Type getType();

    bool isLast();

    size_t getUInt32BufferLength();
    uint32_t *getUInt32Buffer();
    void setUInt32Vector(const QVector<uint32_t> &vector);

    size_t getUInt16BufferLength();
    uint16_t *getUInt16Buffer();

    size_t getUInt8BufferLength();
    uint8_t *getUInt8Buffer();

    size_t getInt8BufferLength();
    int8_t *getInt8Buffer();

    QString getString();
};

typedef QSharedPointer<Message> pMessage;

} // namespace protocol

Q_DECLARE_METATYPE(protocol::pMessage)

#endif // MESSAGE_H
