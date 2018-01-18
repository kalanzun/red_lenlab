#include "utils.h"

namespace model {

QString
german_double(double value)
{
    auto str = QString::number(value);
    str = str.replace(QString("."), QString(","));
    return str;
}

} // namespace model
