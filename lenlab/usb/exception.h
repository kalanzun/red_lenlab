#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>
#include <exception>

namespace usb {

    class Exception : public std::exception
    {
        QString msg;

    public:
        Exception(const QString &msg);

        const QString &getMsg() const;
    };

}

#endif // EXCEPTION_H
