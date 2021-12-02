#ifndef USB_PACKET_H
#define USB_PACKET_H

#include <QDebug>

namespace usb {

class Packet
{
public:
    unsigned char buffer[1024];
    unsigned int length = sizeof(buffer);

    Packet();

    Packet(const Packet&) = delete;
    Packet& operator=(const Packet&) = delete;

    friend QDebug operator<<(QDebug debug, const std::shared_ptr< Packet >& packet);
};

} // namespace usb

#endif // USB_PACKET_H
