#include "parameter.h"

namespace model {

LabelIterator::LabelIterator(const std::function< float(int) > value, const QString label, int i)
    : value{value}
    , label{std::move(label)}
    , i{i}
{

}

bool LabelIterator::operator!=(const LabelIterator& other)
{
    return i != other.i;
}

QString LabelIterator::operator*()
{
    return label.arg(value(i));
}

LabelIterator& LabelIterator::operator++()
{
    ++i;
    return *this;
}

Parameter::Parameter(int length, const std::function< float(int) > value, const QString label)
    : length{length}
    , value{value}
    , label{std::move(label)}
{

}

LabelIterator Parameter::begin() const
{
    return LabelIterator{value, label, 0};
}

LabelIterator Parameter::end() const
{
    return LabelIterator{value, label, length};
}

} // namespace model
