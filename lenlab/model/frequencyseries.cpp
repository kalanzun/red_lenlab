/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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

#include "frequencyseries.h"

#include <cmath>

namespace model {

FrequencySeries::FrequencySeries()
    : Series()
    , index{0, 0, 0}
    , MinY{100.0, -1.0, -90.0}
    , MaxY{10.0e3, 1, 90}
{

}

void
FrequencySeries::append(std::size_t channel, double value)
{
    Q_ASSERT(channel < m_channels);
    Q_ASSERT(index[channel] < m_length);
    if (value < MinY[channel]) MinY[channel] = value;
    if (value > MaxY[channel]) MaxY[channel] = value;
    data[channel][index[channel]++] = value;
}

std::size_t
FrequencySeries::getChannels() const
{
    return m_channels;
}

std::size_t
FrequencySeries::getLength() const
{
    return index[0];
}

double
FrequencySeries::getX(std::size_t i) const
{
    return getY(i, 0);
}

double
FrequencySeries::getY(std::size_t i, std::size_t channel) const
{
    Q_ASSERT(channel < m_channels);
    Q_ASSERT(i < m_length);
    return data[channel][i];
}

double
FrequencySeries::getLastX() const
{
    return getLastY(0);
}

double
FrequencySeries::getLastY(std::size_t channel) const
{
    Q_ASSERT(channel < m_channels);
    Q_ASSERT(index[channel] > 0);
    return data[channel][index[channel] - 1];
}

double
FrequencySeries::getMinX() const
{
    return getMinY(0);
}

double
FrequencySeries::getMaxX() const
{
    return getMaxY(0);
}

double
FrequencySeries::getMinY(std::size_t channel) const
{
    Q_ASSERT(channel < m_channels);
    return MinY[channel];
}

double
FrequencySeries::getMaxY(std::size_t channel) const
{
    Q_ASSERT(channel < m_channels);
    return MaxY[channel];
}

std::size_t
FrequencySeries::startIndex() const
{
    return m_start_index;
}

std::size_t
FrequencySeries::stopIndex() const
{
    return m_stop_index;
}

} // namespace model
