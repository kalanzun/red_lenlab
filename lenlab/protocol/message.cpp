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

namespace protocol {

static int p_message_type_id = qRegisterMetaType<pMessage>("pMessage");

Message::Message() : packet(new usb::Packet())
{

}

Message::Message(const usb::pPacket &packet) : packet(packet)
{

}

usb::pPacket &
Message::getPacket()
{
    return packet;
}

size_t
Message::getHeadLength()
{
    return MESSAGE_HEAD_LENGTH * PACKET_BUFFER_ELEMENT_SIZE;
}

uint8_t *
Message::getHead()
{
    return packet->getByteBuffer();
}

void
Message::setCommand(Command code)
{
    packet->getByteBuffer()[0] = code;
    packet->setByteLength(MESSAGE_HEAD_LENGTH * PACKET_BUFFER_ELEMENT_SIZE);
}

Command
Message::getCommand()
{
    return static_cast<Command>(packet->getByteBuffer()[0]);
}

void
Message::setReply(Reply reply)
{
    packet->getByteBuffer()[0] = reply;
    packet->setByteLength(MESSAGE_HEAD_LENGTH * PACKET_BUFFER_ELEMENT_SIZE);
}

Reply
Message::getReply()
{
    return static_cast<Reply>(packet->getByteBuffer()[0]);
}

void
Message::setType(Type type)
{
    packet->getByteBuffer()[1] = type;
    packet->setByteLength(MESSAGE_HEAD_LENGTH * PACKET_BUFFER_ELEMENT_SIZE);
}

Type
Message::getType()
{
    return static_cast<Type>(packet->getByteBuffer()[1]);
}

bool
Message::isLast()
{
    return static_cast<bool>(packet->getByteBuffer()[2]);
}

size_t
Message::getIntBufferLength()
{
    return (packet->getByteLength() - (MESSAGE_HEAD_LENGTH * PACKET_BUFFER_ELEMENT_SIZE)) / sizeof(uint32_t);
}

uint32_t *
Message::getIntBuffer()
{
    return (packet->getBuffer() + MESSAGE_HEAD_LENGTH);
}

size_t
Message::getShortBufferLength()
{
    return (packet->getByteLength() - (MESSAGE_HEAD_LENGTH * PACKET_BUFFER_ELEMENT_SIZE)) / sizeof(uint16_t);
}

uint16_t *
Message::getShortBuffer()
{
    return reinterpret_cast<uint16_t *>(packet->getBuffer() + MESSAGE_HEAD_LENGTH);
}

size_t
Message::getByteBufferLength()
{
    return (packet->getByteLength() - (MESSAGE_HEAD_LENGTH * PACKET_BUFFER_ELEMENT_SIZE)) / sizeof(uint8_t);
}

uint8_t *
Message::getByteBuffer()
{
    return reinterpret_cast<uint8_t *>(packet->getBuffer() + MESSAGE_HEAD_LENGTH);
}

QString
Message::getString()
{
    auto str = reinterpret_cast<const char *>(packet->getBuffer() + MESSAGE_HEAD_LENGTH);
    Q_ASSERT(*(str + getByteBufferLength() - 1) == 0);
    return QString(str);
}

} // namespace protocol
