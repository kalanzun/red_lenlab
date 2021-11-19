/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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

void
Loggerseries::append(int channel, double value)
{
    Q_ASSERT(channel < m_channels);
    data[channel].append(value);
}

int
Loggerseries::getChannels() const
{
    return m_channels;
}

int
Loggerseries::getLength() const
{
    return data[0].size();
}

double
Loggerseries::getX(int i) const
{
    return static_cast< double >(i * m_interval) / 1000;
}

double
Loggerseries::getY(int i, int channel) const
{
    Q_ASSERT(channel < m_channels);
    Q_ASSERT(i < data[channel].size());
    return data[channel].at(i);
}

double
Loggerseries::getLastX() const
{
    auto length = getLength();
    return length ? getX(length - 1) : 0;
}

double
Loggerseries::getLastY(int channel) const
{
    Q_ASSERT(channel < m_channels);
    return data[channel].size() ? data[channel].last() : 0;
}

double
Loggerseries::getMinX() const
{
    return 0;
}

int
Loggerseries::round_up_towards(int base, int value)
{
    return base * (value / base + (value % base != 0));
}

double
Loggerseries::getMaxX() const
{
    auto value = getLength();
    value = value > 10 ? value - 1 : 10;

    if (value < 30)
        value = round_up_towards(5, value);
    else if (value < 100)
        value = round_up_towards(10, value);
    else if (value < 300)
        value = round_up_towards(50, value);
    else if (value < 1000)
        value = round_up_towards(100, value);
    else if (value < 3000)
        value = round_up_towards(500, value);
    else if (value < 10000)
        value = round_up_towards(1000, value);
    else if (value < 30000)
        value = round_up_towards(5000, value);
    else
        value = round_up_towards(10000, value);

    return getX(value);
}

double
Loggerseries::getMinY(int channel) const
{
    Q_UNUSED(channel);
    return 0;
}

double
Loggerseries::getMaxY(int channel) const
{
    Q_UNUSED(channel);
    return 3.3;
}

} // namespace model
