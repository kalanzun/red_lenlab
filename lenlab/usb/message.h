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
#include <QSharedPointer>

#define MESSAGE_BUFFER_LENGTH 1024

namespace usb {

    class Message
    {
        uint16_t length = MESSAGE_BUFFER_LENGTH;
        uint8_t buffer[MESSAGE_BUFFER_LENGTH];

    public:
        void setCommand(Command command);
        Command getCommand();

        void setReply(Reply reply);
        Reply getReply();

        void setHeader(uint16_t header);
        uint16_t getHeader();

        void setHeader0(uint8_t header0);
        uint8_t getHeader0();

        void setHeader1(uint8_t header1);
        uint8_t getHeader1();

        void setPayloadLength(uint16_t length);
        uint16_t getPayloadLength();

        uint8_t *getPayload();

        void setPacketLength(uint16_t length);
        uint16_t getPacketLength();
        uint8_t *getPacketBuffer();
    };

    typedef QSharedPointer<Message> pMessage;
}

Q_DECLARE_METATYPE(usb::pMessage)

#endif // MESSAGE_H
