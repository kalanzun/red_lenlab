#include "usbexception.h"

namespace usb {

USBException::USBException(ssize_t error)
    : error{static_cast< libusb_error >(error)}
    , message{libusb_strerror(error)}
{

}

USBException::USBException(const char* message)
    : error{LIBUSB_SUCCESS}
    , message{message}
{

}

} // namespace usb
