#include "exception.h"

namespace model {

Exception::Exception(QString const & msg)
    : m_msg(msg)
{

}

QString const &
Exception::getMsg()
{
    return m_msg;
}

} // namespace model
