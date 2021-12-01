#include "transfer.h"

#include <QDebug>

#include "usbexception.h"

namespace usb {

Transfer::Transfer(std::shared_ptr< Packet >& packet, DeviceHandle& device_handle, unsigned char endpoint)
    : m_packet(packet)
{
    xfr = libusb_alloc_transfer(0);
    if (!xfr) throw USBException("Allocation failed");

    xfr->dev_handle = device_handle.m_dev_handle;
    xfr->endpoint = endpoint;
    xfr->type = LIBUSB_TRANSFER_TYPE_BULK;
    xfr->timeout = 0;

    xfr->buffer = m_packet->buffer;
    xfr->length = m_packet->length;
    xfr->user_data = this;
    xfr->callback = &callbackComplete;
}

Transfer::~Transfer()
{
    libusb_free_transfer(xfr);
}

void LIBUSB_CALL Transfer::callbackComplete(struct libusb_transfer* xfr)
{
    auto transfer = static_cast< Transfer* >(xfr->user_data);
    qDebug() << "Transfer::callbackComplete";
    transfer->m_packet->length = xfr->length;
    if (transfer->m_receive_callback) transfer->m_receive_callback(transfer->m_object, transfer->m_packet);
    delete transfer;
}

void Transfer::submit()
{
    // TODO why not inside the constructor?
    auto error = libusb_submit_transfer(xfr);
    if (error) throw USBException(error);
}

} // namespace usb
