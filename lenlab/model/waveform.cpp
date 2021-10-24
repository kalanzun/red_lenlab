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

#include "waveform.h"

#include <QDebug>

namespace model {

Waveform::Waveform()
    : Series()
    , index{0, 0}
{

}

void
Waveform::setSamplerate(double samplerate)
{
    m_samplerate = samplerate;
}

double
Waveform::samplerate()
{
    return m_samplerate;
}

void
Waveform::setTrigger(uint32_t trigger)
{
    m_trigger = trigger;
}

uint32_t
Waveform::trigger()
{
    return m_trigger;
}

void
Waveform::setView(uint32_t view)
{
    Q_ASSERT(view + m_trigger < data.at(0).size());
    m_view = view;
}

uint32_t
Waveform::view()
{
    return m_view;
}

void
Waveform::append(std::size_t channel, double value)
{
    Q_ASSERT(channel < index.size());
    Q_ASSERT(index[channel] < data[channel].size());
    data[channel][index[channel]++] = value;
}

std::size_t
Waveform::getChannels() const
{
    return data.size();
}

std::size_t
Waveform::getLength() const
{
    return m_view;
}

double
Waveform::getX(std::size_t i) const
{
    return static_cast< double >((static_cast< int >(m_view) / -2) + static_cast< int >(i)) / m_samplerate * 1000;
}

double
Waveform::getY(std::size_t i, std::size_t channel) const
{
    Q_ASSERT(channel < data.size());
    Q_ASSERT(m_trigger + i < data[channel].size());
    return data[channel][m_trigger + i];
}

double
Waveform::getMinX() const
{
    return static_cast< double >(static_cast< int >(m_view) / -2) / m_samplerate * 1000;
}

double
Waveform::getMaxX() const
{
    return static_cast< double >(static_cast< int >(m_view) / 2) / m_samplerate * 1000;
}

double
Waveform::getMinY(std::size_t) const
{
    return -2;
}

double
Waveform::getMaxY(std::size_t) const
{
    return 2;
}

} // namespace model
