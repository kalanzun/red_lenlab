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

#include "frequencyseries.h"

#include <cmath>

namespace model {

FrequencySeries::FrequencySeries()
    : Series()
    , index{0, 0}
    , MinY{-20, -90}
    , MaxY{10, 90}
{

}

void
FrequencySeries::append(int channel, double value)
{
    Q_ASSERT(channel < m_channels);
    Q_ASSERT(index[channel] < m_length);
    double delta = channel ? 90 : 30; // phase : magnitude
    while (value < MinY[channel]) MinY[channel] -= delta;
    while (value > MaxY[channel]) MaxY[channel] += delta;
    data[channel][index[channel]++] = value;
}

void
FrequencySeries::appendFrequency(double value)
{
    Q_ASSERT(freq_index < m_length);
    freq[freq_index++] = value;
}

int
FrequencySeries::getChannels() const
{
    return m_channels;
}

int
FrequencySeries::getLength() const
{
    return freq_index;
}

double
FrequencySeries::getX(int i) const
{
    Q_ASSERT(i < m_length);
    return freq[i];
}

double
FrequencySeries::getY(int i, int channel) const
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
FrequencySeries::getLastY(int channel) const
{
    Q_ASSERT(channel < m_channels);
    Q_ASSERT(index[channel]);
    return data[channel][index[channel] - 1];
}

double
FrequencySeries::getMinX() const
{
    return 100;
}

double
FrequencySeries::getMaxX() const
{
    return 10000;
}

double
FrequencySeries::getMinY(int channel) const
{
    Q_ASSERT(channel < m_channels);
    return MinY[channel];
}

double
FrequencySeries::getMaxY(int channel) const
{
    Q_ASSERT(channel < m_channels);
    return MaxY[channel];
}

int
FrequencySeries::startIndex() const
{
    return m_start_index;
}

int
FrequencySeries::stopIndex() const
{
    return m_stop_index;
}

} // namespace model
