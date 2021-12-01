#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include <memory>
#include <functional>

#include "libusb.h"

#include "devicehandle.h"
#include "interface.h"
#include "packet.h"

namespace usb {

class Device
{
    DeviceHandle m_device_handle;
    Interface m_interface;

    static void receiveCallback(void* object, std::shared_ptr< Packet >& packet);
    void receive();

public:
    void* m_object;
    std::function< void(void*, std::shared_ptr< Packet >&) > m_receive_callback;

    Device(libusb_device* dev);
    Device(const Device&) = delete;

    Device& operator=(const Device&) = delete;

    static std::shared_ptr< Device > query(uint16_t vid, uint16_t pid);

    void transmit(std::shared_ptr< Packet >& packet);
};

} // namespace usb

#endif // USB_DEVICE_H
