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

using namespace usb;

int p_message_type_id = qRegisterMetaType<usb::pMessage>("pMessage");

void
Message::setCommand(Command command)
{
    buffer[0] = command;
}

Command Message::getCommand()
{
    return (Command) buffer[0];
}

void
Message::setReply(Reply reply)
{
    buffer[1] = reply;
}

Reply
Message::getReply()
{
    return (Reply) buffer[1];
}

void
Message::setHeader(uint16_t header)
{
    *((uint16_t *) (buffer + 2)) = header;
}

uint16_t
Message::getHeader()
{
    return *((uint16_t *) (buffer + 2));
}

void
Message::setHeader0(uint8_t header0)
{
    buffer[2] = header0;
}

uint8_t
Message::getHeader0()
{
    return buffer[2];
}

void
Message::setHeader1(uint8_t header1)
{
    buffer[3] = header1;
}

uint8_t
Message::getHeader1()
{
    return buffer[3];
}

void
Message::setPayloadLength(uint16_t length)
{
    this->length = length + 4;
}

uint16_t
Message::getPayloadLength()
{
    return length - 4;
}

void
Message::setFullBufferLength()
{
    this->length = MESSAGE_BUFFER_LENGTH;
}

uint8_t *
Message::getPayload()
{
    return buffer + 4;
}

void
Message::setPacketLength(uint16_t length)
{
    this->length = length;
}

uint16_t
Message::getPacketLength()
{
    return length;
}

uint8_t *
Message::getPacketBuffer()
{
    return buffer;
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
