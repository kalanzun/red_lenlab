#include "message.h"

using namespace usb;

int p_message_type_id = qRegisterMetaType<usb::pMessage>("pMessage");

void
Message::setCommand(uint8_t command)
{
    buffer[0] = command;
}

uint8_t
Message::getCommand()
{
    return buffer[0];
}

void
Message::setReply(uint8_t reply)
{
    buffer[1] = reply;
}

uint8_t
Message::getReply()
{
    return buffer[1];
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

uint8_t *
Message::getPayload()
{
    return buffer + 4;
}

uint16_t
Message::getPacketLength()
{
    return length;
}

void
Message::setPacketLength(uint16_t length)
{
    this->length = length;
}

uint8_t *
Message::getPacketBuffer()
{
    return buffer;
}
