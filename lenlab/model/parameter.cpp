#include "parameter.h"

namespace model {

LabelIterator::LabelIterator(const std::function< int(int) > value, const QString label, int i)
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

Parameter::Parameter(int length, const std::function< int(int) > value, const QString label, int default_index)
    : length{length}
    , value{value}
    , label{std::move(label)}
    , default_index{default_index}
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
