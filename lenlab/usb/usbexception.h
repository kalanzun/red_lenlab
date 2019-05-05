#ifndef USBEXCEPTION_H
#define USBEXCEPTION_H

#include <QString>

#include <exception>

namespace usb {

    class USBException : public std::exception
    {
        QString m_msg;

    public:
        USBException(QString const & msg);

        QString const & msg() const;
    };

} // namespace usb

#endif // USBEXCEPTION_H
