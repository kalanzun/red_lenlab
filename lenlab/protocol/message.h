#ifndef PROTOCOL_MESSAGE_H
#define PROTOCOL_MESSAGE_H

#include <memory>

#include "lenlab_protocol.h"

namespace usb { class Packet; }

namespace protocol {

class Message
{
public:
    std::shared_ptr< usb::Packet > const packet;
    struct Head* const head;

    explicit Message();
    explicit Message(std::shared_ptr< usb::Packet >& packet);

    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;

    static std::shared_ptr< Message > createCommand(enum Command command, enum Type type = nullType, uint16_t reference = 0);

    uint8_t* getBuffer();

    uint32_t getInt(int i);
    void addInt(uint32_t value);
};

} // namespace protocol

#endif // PROTOCOL_MESSAGE_H
