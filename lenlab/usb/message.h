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
        uint16_t length = LENLAB_PACKET_HEAD_LENGTH;
        uint8_t buffer[MESSAGE_BUFFER_LENGTH];

    public:
        void setCommand(Command command);
        Command getCommand();

        void setReply(Reply reply);
        Reply getReply();

        void setType(Type type);
        Type getType();

        void setBodyLength(uint32_t length);
        uint32_t getBodyLength();

        void setFullBufferLength();

        uint8_t *getBody();

        uint32_t getPacketLength();
        void setPacketLength(uint32_t length);

        uint8_t *getPacketBuffer();

        const char *getString();
        uint32_t *getIntArray(uint32_t length);

        void setByteArray(uint8_t array[], uint32_t length);
    };

    typedef QSharedPointer<Message> pMessage;

    pMessage newCommand(Command command);
    pMessage newReply();
}

Q_DECLARE_METATYPE(usb::pMessage)

#endif // MESSAGE_H
