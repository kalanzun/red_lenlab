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
Waveform::samplerate() const
{
    return m_samplerate;
}

void
Waveform::setTrigger(int trigger)
{
    Q_ASSERT(trigger < m_packet_length);
    m_trigger = trigger;
}

int
Waveform::trigger() const
{
    return m_trigger;
}

void
Waveform::setView(int view)
{
    Q_ASSERT(view <= m_max_view);
    m_view = view;
}

int
Waveform::view() const
{
    return m_view;
}

void
Waveform::append(int channel, double value)
{
    Q_ASSERT(channel < m_channels);
    Q_ASSERT(index[channel] < m_length);
    data[channel][index[channel]++] = value;
}

int
Waveform::getChannels() const
{
    return m_channels;
}

int
Waveform::getLength() const
{
    return m_view + 1;
}

double
Waveform::getX(int i) const
{
    return static_cast< double >((m_view / -2) + i) / m_samplerate * 1000;
}

double
Waveform::getY(int i, int channel) const
{
    Q_ASSERT(channel < m_channels);

    // i is the index in m_view samples around m_trigger
    // m_trigger is offset by m_max_view / 2
    int index = (m_max_view - m_view) / 2 + m_trigger + i;
    // index is always >= 0
    Q_ASSERT(static_cast< unsigned int >(index) < data[channel].size());

    return data[channel][index];
}

double
Waveform::getMinX() const
{
    return getX(0);
}

double
Waveform::getMaxX() const
{
    return -getX(0);
}

double
Waveform::getMinY(int) const
{
    return -2;
}

double
Waveform::getMaxY(int) const
{
    return 2;
}

} // namespace model
