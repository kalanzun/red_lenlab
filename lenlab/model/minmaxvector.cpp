#include "minmaxvector.h"

namespace model {

void
MinMaxVector::append(double value)
{
    super::append(value);

    if (value < min) min = value;
    if (value > max) max = value;
}

void
MinMaxVector::clear()
{
    super::clear();

    min = 0;
    max = 0;
}

double
MinMaxVector::getMax()
{
    return max;
}

double
MinMaxVector::getMin()
{
    return min;
}

} // namespace model
