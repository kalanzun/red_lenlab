#ifndef USBERROR_H
#define USBERROR_H

#include <QString>

#include <exception>

namespace usb {

class UsbError : public std::exception
{

};

class UsbErrorMessage : public UsbError
{
    QString msg;

public:
    UsbErrorMessage(QString const & msg);

    QString const & getMsg() const;
};

class NotFound : public UsbError
{

};

} // namespace usb

#endif // USBERROR_H
