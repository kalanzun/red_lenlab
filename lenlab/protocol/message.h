/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include "usb/packet.h"

#include "lenlab_protocol.h"

#include <QString>
#include <QVector>

#include <array>
#include <stdint.h>

namespace protocol {

class Message
{
    static size_t const mHeadLength = LENLAB_PACKET_HEAD_LENGTH; // bytes
    usb::pPacket mPacket;

    static std::array<char const *, NUM_COMMANDS> const mCommandNames;
    static std::array<char const *, NUM_REPLIES> const mReplyNames;
    static std::array<char const *, NUM_TYPES> const mTypeNames;

public:
    explicit Message();
    explicit Message(usb::pPacket const &);
    Message(Message const & other) = delete;

    Message & operator=(Message const & other) = delete;

    usb::pPacket & getPacket();

    static size_t getHeadLength();
    uint8_t * getHead();

    void setCommand(Command);
    Command getCommand() const;
    QString getCommandName() const;

    void setReply(Reply reply);
    Reply getReply() const;
    QString getReplyName() const;

    void setType(Type type);
    Type getType() const;
    QString getTypeName() const;

    uint8_t getError() const;

    bool isLast() const;

    size_t getUInt32BufferLength() const;
    uint32_t * getUInt32Buffer();
    void setUInt32Vector(QVector<uint32_t> const & vector);

    size_t getUInt16BufferLength() const;
    uint16_t * getUInt16Buffer();

    size_t getUInt8BufferLength() const;
    uint8_t * getUInt8Buffer();

    size_t getInt8BufferLength() const;
    int8_t * getInt8Buffer();

    QString getString() const;
};

typedef QSharedPointer<Message> pMessage;

} // namespace protocol

Q_DECLARE_METATYPE(protocol::pMessage)

#endif // MESSAGE_H
