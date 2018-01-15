#include "frequencyseries.h"
#include <cmath>

namespace model {

FrequencySeries::FrequencySeries() : Series()
{

}

void
FrequencySeries::append(uint32_t channel, double value)
{
    Q_ASSERT(channel < 2);
    Q_ASSERT(index[channel] < 100);
    if (value < MinY[channel]) MinY[channel] = value;
    if (value > MaxY[channel]) MaxY[channel] = value;
    data[channel][index[channel]++] = value;
}

void
FrequencySeries::clear()
{
    index[0] = 0;
    index[1] = 0;
    MinY[0] = 0.1;
    MinY[1] = -90.0;
    MaxY[0] = 10.0;
    MaxY[1] = 90.0;
}

uint32_t
FrequencySeries::getLength(uint32_t channel)
{
    Q_ASSERT(channel < 2);
    return index[channel];
}

double
FrequencySeries::getX(uint32_t i)
{
    return 20.0 * std::pow(std::pow(10.0, i), 1.0/33.0);
}

double
FrequencySeries::getY(uint32_t i, uint32_t channel)
{
    Q_ASSERT(channel < 2);
    Q_ASSERT(i < 100);
    return data[channel][i];
}

double
FrequencySeries::getMinX()
{
    return 20;
}

double
FrequencySeries::getMaxX()
{
    return 20e3;
}

double
FrequencySeries::getMinY(uint32_t channel)
{
    Q_ASSERT(channel < 2);
    return MinY[channel];
}

double
FrequencySeries::getMaxY(uint32_t channel)
{
    Q_ASSERT(channel < 2);
    return MaxY[channel];
}

} // namespace model
