#include "usbexception.h"

namespace usb {

USBException::USBException(ssize_t error)
    : m_error(static_cast< libusb_error >(error))
    , m_message(nullptr)
{

}

USBException::USBException(const char *message)
    : m_error(LIBUSB_SUCCESS)
    , m_message(message)
{

}

const char *USBException::message() const
{
    if (m_error) return libusb_strerror(m_error);
    else return m_message;
}

} // namespace usb
