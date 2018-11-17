/*

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

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
    if (!xfr) throw Exception("Error allocating transfer");

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
Transfer::start(const pPacket &packet)
{
    std::unique_ptr<NoteToSelf> note(new NoteToSelf(this, &active, packet));

    xfr->buffer = packet->getByteBuffer();
    xfr->length = static_cast<int>(packet->getByteLength());
    xfr->user_data = note.get();
    xfr->callback = callbackComplete;

    auto err = libusb_submit_transfer(xfr.get());
    if (err) throw Exception(libusb_strerror(static_cast<libusb_error>(err)));

    note.release(); // release ownership
}

void
LIBUSB_CALL Transfer::callbackComplete(struct libusb_transfer *xfr)
{
    std::unique_ptr<NoteToSelf> note(static_cast<NoteToSelf *>(xfr->user_data)); // acquire ownership

    switch(xfr->status)
    {
        case LIBUSB_TRANSFER_COMPLETED:
            // Success here, data transfered are inside
            // xfr->buffer
            // and the length is
            // xfr->actual_length
            note->packet->setByteLength(xfr->actual_length);
            emit note->transfer->completed(note->packet);
            break;
        case LIBUSB_TRANSFER_CANCELLED:
            qDebug("USB transfer error: LIBUSB_TRANSFER_CANCELLED");
            emit note->transfer->error("USB transfer error: LIBUSB_TRANSFER_CANCELLED");
            break;
        case LIBUSB_TRANSFER_NO_DEVICE:
            qDebug("USB transfer error: LIBUSB_TRANSFER_NO_DEVICE");
            emit note->transfer->error("USB transfer error: LIBUSB_TRANSFER_NO_DEVICE");
            break;
        case LIBUSB_TRANSFER_TIMED_OUT:
            qDebug("USB transfer error: LIBUSB_TRANSFER_TIMED_OUT");
            emit note->transfer->error("USB transfer error: LIBUSB_TRANSFER_TIMED_OUT");
            break;
        case LIBUSB_TRANSFER_ERROR:
            qDebug("USB transfer error: LIBUSB_TRANSFER_ERROR");
            emit note->transfer->error("USB transfer error: LIBUSB_TRANSFER_ERROR");
            break;
        case LIBUSB_TRANSFER_STALL:
            qDebug("USB transfer error: LIBUSB_TRANSFER_STALL");
            emit note->transfer->error("USB transfer error: LIBUSB_TRANSFER_STALL");
            break;
        case LIBUSB_TRANSFER_OVERFLOW:
            qDebug("USB transfer error: LIBUSB_TRANSFER_OVERFLOW");
            emit note->transfer->error("USB transfer error: LIBUSB_TRANSFER_OVERFLOW");
            break;
    }
}
