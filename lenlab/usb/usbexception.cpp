#include "usbexception.h"

namespace usb {

USBException::USBException(ssize_t error)
    : m_error(static_cast< libusb_error >(error))
    , m_message(libusb_strerror(m_error))
{

}

USBException::USBException(const char* message)
    : m_error(LIBUSB_SUCCESS)
    , m_message(message)
{

}

} // namespace usb
