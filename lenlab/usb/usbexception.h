#ifndef USB_USBEXCEPTION_H
#define USB_USBEXCEPTION_H

#include <QDebug>

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

    friend QDebug operator<<(QDebug debug, const USBException& exception);
};

} // namespace usb

#endif // USB_USBEXCEPTION_H
