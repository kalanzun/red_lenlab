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
    , index{0, 0}
    , MinY{-1.0, -90.0}
    , MaxY{1, 90}
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

void
FrequencySeries::appendFrequency(double value)
{
    Q_ASSERT(freq_index < m_length);
    freq[freq_index++] = value;
}

std::size_t
FrequencySeries::getChannels() const
{
    return m_channels;
}

std::size_t
FrequencySeries::getLength() const
{
    return freq_index;
}

double
FrequencySeries::getX(std::size_t i) const
{
    Q_ASSERT(i < m_length);
    return freq[i];
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
    Q_ASSERT(freq_index);
    return freq[freq_index - 1];
}

double
FrequencySeries::getLastY(std::size_t channel) const
{
    Q_ASSERT(channel < m_channels);
    Q_ASSERT(index[channel]);
    return data[channel][index[channel] - 1];
}

double
FrequencySeries::getMinX() const
{
    return freq_index ? freq[0] : 1;
}

double
FrequencySeries::getMaxX() const
{
    return freq_index ? freq[freq_index - 1] : 1;
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
