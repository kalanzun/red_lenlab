#ifndef PROTOCOL_MESSAGE_H
#define PROTOCOL_MESSAGE_H

#include <memory>

#include "lenlab_protocol.h"

namespace usb { class Packet; }

namespace protocol {

struct Page {
    struct Head head;
    uint16_t log2oversamples;
    uint16_t trigger_config;
    uint16_t channel;
    uint16_t index;
    uint16_t trigger;
    uint16_t res[5];
    uint16_t values[500];
};

class Message
{
public:
    std::shared_ptr< usb::Packet > const packet;
    struct Head* const head;
    struct Page* const page;

    explicit Message();
    explicit Message(std::shared_ptr< usb::Packet > packet);

    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;

    static std::shared_ptr< Message > createCommand(enum Command command, enum Type type = nullType, uint16_t reference = 0);
    static std::shared_ptr< Message > createReply(enum Reply reply, enum Type type = nullType, uint16_t reference = 0);

    uint8_t* getBuffer() const;

    uint32_t getInt(int i) const;
    void addInt(uint32_t value);
};

} // namespace protocol

#endif // PROTOCOL_MESSAGE_H
