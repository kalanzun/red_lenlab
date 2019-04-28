#include "frequencyseries.h"

#include <cmath>

namespace model {

FrequencySeries::FrequencySeries() : Series()
{
    clear();
}

void
FrequencySeries::append(uint32_t channel, double value)
{
    Q_ASSERT(channel < channel_length);
    Q_ASSERT(index[channel] < values_length);
    if (value < MinY[channel]) MinY[channel] = value;
    if (value > MaxY[channel]) MaxY[channel] = value;
    data[channel][index[channel]++] = value;
}

void
FrequencySeries::clear()
{
    Q_ASSERT(channel_length == 3);

    index[0] = 0;
    index[1] = 0;
    index[2] = 0;

    MinY[0] = 100.0;
    MinY[1] = -1.0;
    MinY[2] = -90.0;

    MaxY[0] = 10.0e3;
    MaxY[1] = 1;
    MaxY[2] = 90;
}

uint32_t
FrequencySeries::getLength(uint32_t channel)
{
    Q_ASSERT(channel < channel_length);
    return index[channel];
}

double
FrequencySeries::getX(uint32_t i)
{
    return getY(i, 0);
}

double
FrequencySeries::getY(uint32_t i, uint32_t channel)
{
    Q_ASSERT(channel < channel_length);
    Q_ASSERT(i < values_length);
    return data[channel][i];
}

double
FrequencySeries::getMinX()
{
    return getMinY(0);
}

double
FrequencySeries::getMaxX()
{
    return getMaxY(0);
}

double
FrequencySeries::getMinY(uint32_t channel)
{
    Q_ASSERT(channel < channel_length);
    return MinY[channel];
}

double
FrequencySeries::getMaxY(uint32_t channel)
{
    Q_ASSERT(channel < channel_length);
    return MaxY[channel];
}

} // namespace model
