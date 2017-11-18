#include "exception.h"

using namespace usb;

Exception::Exception(const QString &msg) : std::exception(), msg(msg)
{

}

const QString &
Exception::getMsg() const
{
    return msg;
}
