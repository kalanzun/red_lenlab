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
    return LENLAB_PACKET_HEAD_LENGTH;
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
    for (int i = 1; i < LENLAB_PACKET_HEAD_LENGTH; i++)
        packet->getByteBuffer()[i] = 0;
    packet->setByteLength(LENLAB_PACKET_HEAD_LENGTH);
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
    for (int i = 1; i < LENLAB_PACKET_HEAD_LENGTH; i++)
        packet->getByteBuffer()[i] = 0;
    packet->setByteLength(LENLAB_PACKET_HEAD_LENGTH);
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
}

Type
Message::getType()
{
    return static_cast<Type>(packet->getByteBuffer()[1]);
}

bool
Message::isLast()
{
    return static_cast<bool>(packet->getByteBuffer()[3]);
}

size_t
Message::getUInt32BufferLength()
{
    return (packet->getByteLength() - LENLAB_PACKET_HEAD_LENGTH) / sizeof(uint32_t);
}

uint32_t *
Message::getUInt32Buffer()
{
    return (packet->getBuffer() + LENLAB_PACKET_HEAD_LENGTH / 4);
}

void
Message::setUInt32Vector(const QVector<uint32_t> &vector)
{
    setType(IntArray);
    for (int i = 0; i < vector.size(); ++i)
        getUInt32Buffer()[i] = vector.at(i);
    packet->setByteLength((LENLAB_PACKET_HEAD_LENGTH) + (static_cast<size_t>(vector.size()) * sizeof(uint32_t)));
}

size_t
Message::getUInt16BufferLength()
{
    return (packet->getByteLength() - LENLAB_PACKET_HEAD_LENGTH) / sizeof(uint16_t);
}

uint16_t *
Message::getUInt16Buffer()
{
    return reinterpret_cast<uint16_t *>(packet->getBuffer() + LENLAB_PACKET_HEAD_LENGTH / 4);
}

size_t
Message::getUInt8BufferLength()
{
    return (packet->getByteLength() - LENLAB_PACKET_HEAD_LENGTH) / sizeof(uint8_t);
}

uint8_t *
Message::getUInt8Buffer()
{
    return reinterpret_cast<uint8_t *>(packet->getBuffer() + LENLAB_PACKET_HEAD_LENGTH / 4);
}

size_t
Message::getInt8BufferLength()
{
    return (packet->getByteLength() - LENLAB_PACKET_HEAD_LENGTH) / sizeof(int8_t);
}

int8_t *
Message::getInt8Buffer()
{
    return reinterpret_cast<int8_t *>(packet->getBuffer() + LENLAB_PACKET_HEAD_LENGTH / 4);
}

QString
Message::getString()
{
    auto str = reinterpret_cast<const char *>(packet->getBuffer() + LENLAB_PACKET_HEAD_LENGTH / 4);
    Q_ASSERT(*(str + getUInt8BufferLength() - 1) == 0);
    return QString(str);
}

} // namespace protocol
