#include "transfer.h"
#include "exception.h"
#include "notetoself.h"
#include <QDebug>

using namespace usb;

Transfer::Transfer(libusb_device_handle *dev_handle, unsigned char endpoint, QObject *parent) :
    QObject(parent),
    active(),
    xfr(libusb_alloc_transfer(0), libusb_free_transfer)
{
    if (!xfr) throw Exception();

    xfr->dev_handle = dev_handle;
    xfr->endpoint = endpoint;
    xfr->type = LIBUSB_TRANSFER_TYPE_BULK;
    xfr->timeout = 0;
}

Transfer::~Transfer()
{
    libusb_cancel_transfer(xfr.get());
    active.wait();
}

bool
Transfer::isActive()
{
    return active.isActive();
}

void
Transfer::start(const pMessage &message)
{
    std::unique_ptr<NoteToSelf> note(new NoteToSelf(this, &active, message));

    xfr->buffer = message->getPacketBuffer();
    xfr->length = message->getPacketLength();
    xfr->user_data = note.get();
    xfr->callback = callbackComplete;
    if(libusb_submit_transfer(xfr.get()) < 0) throw Exception();

    note.release(); // release ownership
}

void
LIBUSB_CALL Transfer::callbackComplete(struct libusb_transfer *xfr)
{
    std::unique_ptr<NoteToSelf> note((NoteToSelf *) xfr->user_data); // acquire ownership

    switch(xfr->status)
    {
        case LIBUSB_TRANSFER_COMPLETED:
            // Success here, data transfered are inside
            // xfr->buffer
            // and the length is
            // xfr->actual_length
            note->message->setPacketLength(xfr->actual_length);
            emit note->transfer->completed(note->message);
            break;
        case LIBUSB_TRANSFER_CANCELLED:
            qDebug("USBTransfer error: LIBUSB_TRANSFER_CANCELLED");
            //emit error();
            break;
        case LIBUSB_TRANSFER_NO_DEVICE:
            qDebug("USBTransfer error LIBUSB_TRANSFER_NO_DEVICE");
            //emit error();
            break;
        case LIBUSB_TRANSFER_TIMED_OUT:
            qDebug("USBTransfer error LIBUSB_TRANSFER_TIMED_OUT");
            //emit error();
            break;
        case LIBUSB_TRANSFER_ERROR:
            qDebug("USBTransfer error LIBUSB_TRANSFER_ERROR");
            //emit error();
            break;
        case LIBUSB_TRANSFER_STALL:
            qDebug("USBTransfer error LIBUSB_TRANSFER_STALL");
            //emit error();
            break;
        case LIBUSB_TRANSFER_OVERFLOW:
            qDebug("USBTransfer error LIBUSB_TRANSFER_OVERFLOW");
            //emit error();
            break;
    }
}
