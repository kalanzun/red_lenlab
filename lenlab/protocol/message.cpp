#include "message.h"

#include "assert.h"

#include "usb/packet.h"

namespace protocol {

Message::Message()
    : packet{std::make_shared< usb::Packet >()}
    , head{(struct Head*) packet->buffer}
{

}

Message::Message(std::shared_ptr< usb::Packet >& packet)
    : packet{packet}
    , head{(struct Head*) packet->buffer}
{

}

std::shared_ptr< Message > Message::createCommand(enum Command command, enum Type type, uint16_t reference)
{
    auto message = std::make_shared< Message >();

    message->head->command = command;
    message->head->type = type;
    message->head->reference = reference;

    message->packet->length = sizeof(struct Head);

    return message;
}

uint32_t Message::getInt(int i)
{
    assert(head->type == IntArray);
    assert(sizeof(struct Head) + i * sizeof(uint32_t) < packet->length);

    return *(uint32_t *) (packet->buffer + sizeof(struct Head) + i * sizeof(uint32_t));
}

void Message::addInt(uint32_t value)
{
    assert(head->type == IntArray);

    *(uint32_t *) (packet->buffer + packet->length) = value;
    packet->length += sizeof(uint32_t);
}

} // namespace protocol
