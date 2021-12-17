#include "parameter.h"

namespace model {

LabelIterator::LabelIterator(const std::function< int(int) > value, const QString label, int index)
    : value{value}
    , label{std::move(label)}
    , index{index}
{

}

bool LabelIterator::operator!=(const LabelIterator& other)
{
    return index != other.index;
}

QString LabelIterator::operator*()
{
    return label.arg(value(index));
}

LabelIterator& LabelIterator::operator++()
{
    ++index;
    return *this;
}

Parameter::Parameter(int length, const std::function< int(int) > value, const QString label, int index, QObject *parent)
    : QObject{parent}
    , length{length}
    , value{value}
    , label{std::move(label)}
    , index{index}
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

int Parameter::getIndex() const
{
    return index;
}

void Parameter::setIndex(int index)
{
    assert(index >= 0);
    assert(index < length);
    this->index = index;
}

int Parameter::getValue() const
{
    return value(index);
}

} // namespace model
