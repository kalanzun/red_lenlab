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

QDebug operator<<(QDebug debug, const USBException& exception)
{
    //QDebugStateSaver saver(debug);
    debug.nospace() << "USBException(\"" << exception.message << "\")";
    return debug;
}

} // namespace usb
