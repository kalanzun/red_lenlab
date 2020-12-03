/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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

#include "packet.h"

using namespace usb;

static int p_packet_type_id = qRegisterMetaType<usb::pPacket>("pPacket");

Packet::Packet()
{

}

size_t
Packet::getByteLength()
{
    return byte_length;
}

void
Packet::setByteLength(size_t length)
{
    byte_length = length;
}

uint32_t *
Packet::getBuffer()
{
    return buffer;
}

uint8_t *
Packet::getByteBuffer()
{
    return reinterpret_cast<uint8_t *>(buffer);
}

void
Packet::enableMockSendError()
{
    mock_send_error = true;
}

bool
Packet::getMockSendError()
{
    return mock_send_error;
}
