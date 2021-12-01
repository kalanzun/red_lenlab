#ifndef USB_TRANSFER_H
#define USB_TRANSFER_H

#include <functional>
#include <memory>

#include "libusb.h"

#include "devicehandle.h"
#include "packet.h"

namespace usb {

class Transfer
{
    struct libusb_transfer* xfr;
    std::shared_ptr< Packet > m_packet;

    static void LIBUSB_CALL callbackComplete(struct libusb_transfer* xfr);

public:
    void* m_object;
    std::function< void(void*, std::shared_ptr< Packet >&) > m_receive_callback;

    Transfer(std::shared_ptr< Packet >& packet, DeviceHandle& device_handle, unsigned char endpoint);
    Transfer(const Transfer&) = delete;

    ~Transfer();

    Transfer& operator=(const Transfer&) = delete;

    void submit();
};

} // namespace usb

#endif // USB_TRANSFER_H
