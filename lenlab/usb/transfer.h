#ifndef USB_TRANSFER_H
#define USB_TRANSFER_H

#include <functional>
#include <memory>
#include <mutex>

#include "libusb.h"

namespace usb {

class Interface;
class Packet;
class Transfer;

class TransferCallback
{
    std::function< void(Transfer*, void*) > callback = nullptr;
    void* object = nullptr;

public:
    explicit TransferCallback();

    TransferCallback(const TransferCallback&) = delete;
    TransferCallback& operator=(const TransferCallback&) = delete;

    void set(const std::function< void(Transfer*, void*) >& callback, void* object);
    void call(Transfer *transfer) const;
};

class Transfer
{
    std::shared_ptr< Interface > interface; // keep it around while xfr has a dev_handle pointer
    struct libusb_transfer* xfr;

    std::mutex active;

public:
    TransferCallback complete_callback;
    TransferCallback error_callback;

    std::shared_ptr< Packet > packet;

    explicit Transfer(std::shared_ptr< Interface > interface, unsigned char endpoint);
    ~Transfer();

    Transfer(const Transfer&) = delete;
    Transfer& operator=(const Transfer&) = delete;

    void submit(std::shared_ptr< Packet > packet);
    void submit();
    void cancel();

    const char* getErrorMessage() const;

private:
    static void LIBUSB_CALL callbackComplete(struct libusb_transfer* xfr);
};

} // namespace usb

#endif // USB_TRANSFER_H
