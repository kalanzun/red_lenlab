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

#ifndef PACKET_H
#define PACKET_H

#include <QSharedPointer>
#include <stdint.h>

#define PACKET_BUFFER_BYTE_LENGTH 1024
#define PACKET_BUFFER_ELEMENT_SIZE sizeof(uint32_t)
#define PACKET_HEAD_LENGTH 1

namespace usb {

    class Packet
    {
        int byte_length;
        uint32_t buffer[PACKET_BUFFER_BYTE_LENGTH / PACKET_BUFFER_ELEMENT_SIZE];
        // this uses a large type, because only casts to shorter types are ok with alignment

    public:
        Packet(int byte_length);

        int getByteLength();
        void setByteLength(int length);

        uint8_t *getByteBuffer();

        uint8_t *getHead();
        uint32_t *getIntBody();
        uint16_t *getShortBody();
        uint8_t *getByteBody();
    };

    typedef QSharedPointer<Packet> pPacket;

    pPacket newCommand(unsigned char code);
    pPacket newReply();

} // namespace usb

Q_DECLARE_METATYPE(usb::pPacket)

#endif // PACKET_H
