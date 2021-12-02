#ifndef USB_TRANSFER_H
#define USB_TRANSFER_H

#include <functional>
#include <memory>
#include <mutex>

#include "libusb.h"

#include "interface.h"
#include "packet.h"

namespace usb {

class Transfer;

class TransferCallback
{
    std::function< void(Transfer*, void*) > callback = nullptr;
    void* object = nullptr;

public:
    TransferCallback();
    TransferCallback(const TransferCallback&) = delete;

    TransferCallback& operator=(const TransferCallback&) = delete;

    void set(std::function< void(Transfer*, void*) > callback, void* object);
    void call(Transfer *self);
};

class Transfer
{
    std::shared_ptr< Interface > interface; // keep it around while xfr has a dev_handle pointer
    struct libusb_transfer* xfr;

    std::mutex active;
    std::shared_ptr< Packet > current;

public:
    Transfer(std::shared_ptr< Interface > interface, unsigned char endpoint);
    Transfer(const Transfer&) = delete;

    ~Transfer();

    Transfer& operator=(const Transfer&) = delete;

    TransferCallback complete_callback;
    TransferCallback error_callback;

    void submit(std::shared_ptr< Packet > packet);
    void cancel();

    std::shared_ptr< Packet > getPacket();
    const char* getMessage();

private:
    static void LIBUSB_CALL callbackComplete(struct libusb_transfer* xfr);
};

} // namespace usb

#endif // USB_TRANSFER_H
