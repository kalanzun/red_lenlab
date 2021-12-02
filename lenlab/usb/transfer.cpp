#include "transfer.h"

#include "usbexception.h"

namespace usb {

TransferCallback::TransferCallback()
{

}

void TransferCallback::set(std::function< void(Transfer*, void*) > callback, void* object)
{
    this->callback = callback;
    this->object = object;
}

void TransferCallback::call(Transfer *transfer)
{
    if (callback) callback(transfer, object);
}

Transfer::Transfer(std::shared_ptr< Interface > interface, unsigned char endpoint)
    : interface{std::move(interface)}
{
    xfr = libusb_alloc_transfer(0);
    if (!xfr) throw USBException("Allocation failed");

    active.lock();

    xfr->dev_handle = this->interface->device_handle->dev_handle;
    xfr->endpoint = endpoint;
    xfr->type = LIBUSB_TRANSFER_TYPE_BULK;
    xfr->callback = callbackComplete;
    xfr->user_data = this;
    xfr->timeout = 0;
}

Transfer::~Transfer()
{
    // wait for LIBUSB_TRANSFER_CANCELLED callback
    std::lock_guard< std::mutex > lock(active);
    libusb_free_transfer(xfr);
}

void Transfer::submit(std::shared_ptr< Packet > packet)
{
    this->packet = std::move(packet); // keep the packet around during the transfer
    xfr->buffer = this->packet->buffer;
    xfr->length = this->packet->length;

    auto error = libusb_submit_transfer(xfr);
    if (error) throw USBException(error);
}

void Transfer::cancel()
{
    if (libusb_cancel_transfer(xfr)) active.unlock(); // if it can't cancel, unlock it right away
}

const char* Transfer::getErrorMessage()
{
    switch(xfr->status) {
    case LIBUSB_TRANSFER_COMPLETED:
        return "LIBUSB_TRANSFER_COMPLETED";
    case LIBUSB_TRANSFER_CANCELLED:
        return "LIBUSB_TRANSFER_CANCELLED";
    case LIBUSB_TRANSFER_NO_DEVICE:
        return "LIBUSB_TRANSFER_NO_DEVICE";
    case LIBUSB_TRANSFER_TIMED_OUT:
        return "LIBUSB_TRANSFER_TIMED_OUT";
    case LIBUSB_TRANSFER_ERROR:
        return "LIBUSB_TRANSFER_ERROR";
    case LIBUSB_TRANSFER_STALL:
        return "LIBUSB_TRANSFER_STALL";
    case LIBUSB_TRANSFER_OVERFLOW:
        return "LIBUSB_TRANSFER_OVERFLOW";
    default:
        return "";
    }
}

void LIBUSB_CALL Transfer::callbackComplete(struct libusb_transfer* xfr)
{
    auto transfer = static_cast< Transfer* >(xfr->user_data);

    switch(xfr->status) {
    case LIBUSB_TRANSFER_COMPLETED:
        transfer->packet->length = xfr->actual_length;
        transfer->complete_callback.call(transfer);
        break;
    case LIBUSB_TRANSFER_CANCELLED:
        transfer->active.unlock();
        break;
    case LIBUSB_TRANSFER_NO_DEVICE:
    case LIBUSB_TRANSFER_TIMED_OUT:
    case LIBUSB_TRANSFER_ERROR:
    case LIBUSB_TRANSFER_STALL:
    case LIBUSB_TRANSFER_OVERFLOW:
        transfer->error_callback.call(transfer);
        break;
    }
}

} // namespace usb
