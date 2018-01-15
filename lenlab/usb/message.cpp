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

int p_message_type_id = qRegisterMetaType<usb::pMessage>("pMessage");

void
Message::setCommand(Command command)
{
    buffer[0] = command;
}

Command
Message::getCommand()
{
    Q_ASSERT(buffer[0] < NUM_COMMANDS);
    return (Command) buffer[0];
}

void
Message::setReply(Reply reply)
{
    buffer[0] = reply;
}

Reply
Message::getReply()
{
    if (!(buffer[0] < NUM_REPLIES))
        return noReply;

    return (Reply) buffer[0];
}

void
Message::setType(Type type)
{
    buffer[1] = type;
}

Type
Message::getType()
{
    Q_ASSERT(buffer[1] < NUM_TYPES);
    return (Type) buffer[1];
}

void
Message::setBodyLength(uint32_t length)
{
    Q_ASSERT(length <= LENLAB_PACKET_BODY_LENGTH);
    this->length = LENLAB_PACKET_HEAD_LENGTH + length;
}

uint32_t
Message::getBodyLength()
{
    return length - LENLAB_PACKET_HEAD_LENGTH;
}

void
Message::setFullBufferLength()
{
    this->length = MESSAGE_BUFFER_LENGTH;
}

uint8_t *
Message::getBody()
{
    return buffer + LENLAB_PACKET_HEAD_LENGTH;
}

void
Message::setPacketLength(uint32_t length)
{
    Q_ASSERT(length <= MESSAGE_BUFFER_LENGTH);
    this->length = length;
}

uint32_t
Message::getPacketLength()
{
    return length;
}

uint8_t *
Message::getPacketBuffer()
{
    return buffer;
}

const char *
Message::getString()
{
    Q_ASSERT(getType() == String);
    Q_ASSERT(getBody()[getBodyLength()-1] == 0);
    return (const char *) getBody();
}

uint32_t *
Message::getIntArray(uint32_t length)
{
    Q_ASSERT(getType() == IntArray);
    Q_ASSERT(getBodyLength() == 4*length);
    return (uint32_t *) getBody();
}

void
Message::setByteArray(uint8_t array[], uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
        getBody()[i] = array[i];
    setType(ByteArray);
    setBodyLength(length);
}

void
Message::setByte(uint8_t value)
{
    setByteArray(&value, 1);
}

void
Message::setInt(uint32_t value)
{
    Q_ASSERT(getType() == IntArray);
    *(uint32_t *) (getBody() + getBodyLength()) = value;
    setBodyLength(getBodyLength() + 4);
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
