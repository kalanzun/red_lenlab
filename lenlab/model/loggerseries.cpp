#include "loggerseries.h"

namespace model {

Loggerseries::Loggerseries()
{

}

void
Loggerseries::setInterval(uint32_t interval)
{
    m_interval = interval;
}

uint32_t
Loggerseries::interval() const
{
    return m_interval;
}

void Loggerseries::append(uint32_t channel, double value)
{
    data[channel].append(value);
}

uint32_t Loggerseries::getLength(uint32_t channel)
{
    return data[channel].size();
}

double Loggerseries::getX(uint32_t i)
{
    return static_cast<double>(i * m_interval / 1000);
}

double Loggerseries::getY(uint32_t i, uint32_t channel)
{
    return data[channel].at(i);
}

double Loggerseries::getMinX()
{
    return 0;
}

double Loggerseries::getMaxX()
{
    auto length = getLength(0);
    if (length > 0)
        return getX(length - 1);
    else
        return 0;
}

double Loggerseries::getMinY(uint32_t channel)
{
    return 0;
}

double Loggerseries::getMaxY(uint32_t channel)
{
    return 3.5;
}

} // namespace model
