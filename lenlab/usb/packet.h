#ifndef USB_PACKET_H
#define USB_PACKET_H

namespace usb {

class Packet
{
public:
    unsigned char buffer[1024];
    unsigned int length = sizeof(buffer);

    Packet();

    Packet(const Packet&) = delete;
    Packet& operator=(const Packet&) = delete;
};

} // namespace usb

#endif // USB_PACKET_H
