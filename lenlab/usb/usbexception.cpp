#include "usbexception.h"

namespace usb {

USBException::USBException(QString const & msg)
    : m_msg(msg)
{

}

QString const &
USBException::msg() const
{
    return m_msg;
}

} // namespace usb
