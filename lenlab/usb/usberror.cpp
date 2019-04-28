#include "usberror.h"

namespace usb {

UsbErrorMessage::UsbErrorMessage(QString const & msg)
    : msg(msg)
{

}

QString const & UsbErrorMessage::getMsg() const
{
    return msg;
}

} // namespace usb
