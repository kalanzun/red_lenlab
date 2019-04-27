#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>

#include <exception>

namespace model {

class Exception : public std::exception
{
    QString m_msg;

public:
    Exception(QString const & msg);

    QString const & getMsg();
};

} // namespace model

#endif // EXCEPTION_H
