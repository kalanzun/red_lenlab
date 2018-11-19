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

#include "message.h"
#include <QDebug>

using namespace usb;

static int p_message_type_id = qRegisterMetaType<usb::pMessage>("pMessage");

void
Message::setPacketLength(int length)
{
    Q_ASSERT(length <= 4 * MESSAGE_BUFFER_LENGTH);
    this->length = static_cast<uint32_t>(length);
}

int
Message::getPacketLength()
{
    return static_cast<int>(length);
}

unsigned char *
Message::getPacketBuffer()
{
    return reinterpret_cast<unsigned char*>(buffer);
}

uint32_t *
Message::getHead()
{
    return buffer;
}

uint8_t *
Message::getHeadByte()
{
    return reinterpret_cast<uint8_t *>(getHead());
}

uint32_t *
Message::getBody()
{
    return buffer + (LENLAB_PACKET_HEAD_LENGTH / 4);
}

uint16_t *
Message::getBodyShort()
{
    return reinterpret_cast<uint16_t *>(getBody());
}

uint8_t *
Message::getBodyByte()
{
    return reinterpret_cast<uint8_t *>(getBody());
}

void
Message::setCommand(Command command)
{
    getHeadByte()[0] = command;
}

Command
Message::getCommand()
{
    auto bytes = getHeadByte();
    Q_ASSERT(bytes[0] < NUM_COMMANDS);
    return static_cast<Command>(bytes[0]);
}

void
Message::setReply(Reply reply)
{
    getHeadByte()[0] = reply;
}

Reply
Message::getReply()
{
    auto bytes = getHeadByte();

    if (!(bytes[0] < NUM_REPLIES))
        return noReply;

    return static_cast<Reply>(bytes[0]);
}

void
Message::setType(Type type)
{
    getHeadByte()[1] = type;
}

Type
Message::getType()
{
    auto bytes = getHeadByte();
    Q_ASSERT(bytes[1] < NUM_TYPES);
    return static_cast<Type>(bytes[1]);
}

void
Message::setBodyLength(uint32_t length)
{
    Q_ASSERT(length <= LENLAB_PACKET_BODY_LENGTH);
    this->length = LENLAB_PACKET_HEAD_LENGTH + length;
}

uint32_t Message::getBodyLength()
{
    return length - LENLAB_PACKET_HEAD_LENGTH;
}

void
Message::setFullBufferLength()
{
    this->length = MESSAGE_BUFFER_LENGTH * 4;
}

const char *
Message::getString()
{
    Q_ASSERT(getType() == String);
    Q_ASSERT(getBody()[getBodyLength()-1] == 0);
    return reinterpret_cast<const char *>(getBody());
}

uint32_t *
Message::getIntArray(uint32_t length)
{
    Q_ASSERT(getType() == IntArray);
    Q_ASSERT(getBodyLength() == 4*length);
    return getBody();
}

void
Message::setIntArray(uint32_t array[], uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
        getBody()[i] = array[i];
    setType(IntArray);
    setBodyLength(4*length);
}

uint16_t *
Message::getShortArray(uint32_t length)
{
    Q_ASSERT(getType() == ShortArray);
    Q_ASSERT(getBodyLength() == 2*length);
    return getBodyShort();
}

void
Message::setShortArray(uint16_t array[], uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
        getBodyShort()[i] = array[i];
    setType(ShortArray);
    setBodyLength(2*length);
}

uint8_t *
Message::getByteArray(uint32_t length)
{
    Q_ASSERT(getType() == ByteArray);
    Q_ASSERT(getBodyLength() == length);
    return getBodyByte();
}

void
Message::setByteArray(uint8_t array[], uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
        getBodyByte()[i] = array[i];
    setType(ByteArray);
    setBodyLength(length);
}

uint32_t
Message::getInt(uint32_t index)
{
    Q_ASSERT(getType() == IntArray);
    Q_ASSERT(getBodyLength() >= (4 * (index + 1)));
    return getBody()[index];
}

void
Message::setInt(uint32_t index, uint32_t value)
{
    getBody()[index] = value;
    setType(IntArray);
    if (getBodyLength() < (4 * (index + 1)))
        setBodyLength(4 * (index + 1));
}

uint8_t
Message::getByte(uint32_t index)
{
    Q_ASSERT(getType() == ByteArray);
    Q_ASSERT(getBodyLength() >= (index + 1));
    return getBodyByte()[index];
}

void
Message::setByte(uint32_t index, uint8_t value)
{
    getBodyByte()[index] = value;
    setType(ByteArray);
    if (getBodyLength() < (index + 1))
        setBodyLength(index + 1);
}

pMessage
usb::newCommand(Command command_code)
{
    pMessage cmd(new Message());
    cmd->setCommand(command_code);
    return cmd;
}

pMessage
usb::newReply()
{
    pMessage reply(new Message());
    reply->setFullBufferLength();
    return reply;
}
