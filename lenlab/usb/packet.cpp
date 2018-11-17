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

#include "packet.h"

using namespace usb;

static int p_packet_type_id = qRegisterMetaType<usb::pPacket>("pPacket");

Packet::Packet(int byte_length) : byte_length(byte_length)
{

}

int
Packet::getByteLength()
{
    return byte_length;
}

void
Packet::setByteLength(int length)
{
    byte_length = length;
}

uint8_t *
Packet::getByteBuffer()
{
    return reinterpret_cast<uint8_t *>(buffer);
}

uint8_t *
Packet::getHead()
{
    return reinterpret_cast<uint8_t *>(buffer);
}

uint32_t *
Packet::getIntBody()
{
    return buffer + PACKET_HEAD_LENGTH;
}

uint16_t *
Packet::getShortBody()
{
    return reinterpret_cast<uint16_t *>(buffer + PACKET_HEAD_LENGTH);
}

uint8_t *
Packet::getByteBody()
{
    return reinterpret_cast<uint8_t *>(buffer + PACKET_HEAD_LENGTH);
}

pPacket
usb::newCommand(unsigned char code)
{
    pPacket cmd(new Packet(PACKET_HEAD_LENGTH * PACKET_BUFFER_ELEMENT_SIZE));
    cmd->getByteBuffer()[0] = code;
    return cmd;
}

pPacket
usb::newReply()
{
    pPacket reply(new Packet(PACKET_BUFFER_BYTE_LENGTH));
    return reply;
}
