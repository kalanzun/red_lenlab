/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
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

#ifndef PACKET_H
#define PACKET_H

#include <QSharedPointer>
#include <stdint.h>

#define PACKET_BUFFER_BYTE_LENGTH 1024
#define PACKET_BUFFER_ELEMENT_SIZE sizeof(uint32_t)

namespace usb {

    class Packet
    {
        size_t byte_length = PACKET_BUFFER_BYTE_LENGTH;
        uint32_t buffer[PACKET_BUFFER_BYTE_LENGTH / PACKET_BUFFER_ELEMENT_SIZE];
        // this uses a large type, because only casts to shorter types are ok with alignment

        bool mock_send_error = false;

    public:
        Packet();

        size_t getByteLength();
        void setByteLength(size_t length);

        uint32_t *getBuffer();
        uint8_t *getByteBuffer();

        void enableMockSendError();
        bool getMockSendError();
    };

    typedef QSharedPointer<Packet> pPacket;

} // namespace usb

Q_DECLARE_METATYPE(usb::pPacket)

#endif // PACKET_H
