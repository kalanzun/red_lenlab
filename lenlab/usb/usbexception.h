#ifndef USB_USBEXCEPTION_H
#define USB_USBEXCEPTION_H

#include "libusb.h"

namespace usb {

class USBException
{
    libusb_error m_error;
    const char *m_message;

public:
    USBException(ssize_t error);
    USBException(const char *message);
    USBException(const USBException&) = delete;

    USBException& operator=(const USBException&) = delete;

    const char *message() const;
};

} // namespace usb

#endif // USB_USBEXCEPTION_H
