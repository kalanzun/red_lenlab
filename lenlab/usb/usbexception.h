#ifndef USB_USBEXCEPTION_H
#define USB_USBEXCEPTION_H

#include "libusb.h"

namespace usb {

class USBException
{
public:
    libusb_error error;
    const char* message;

    USBException(ssize_t error);
    USBException(const char* message);

    USBException(const USBException&) = delete;
    USBException& operator=(const USBException&) = delete;
};

} // namespace usb

#endif // USB_USBEXCEPTION_H
