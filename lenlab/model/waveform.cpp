/*

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "waveform.h"
#include <QDebug>

namespace model {

Waveform::Waveform() : Series()
{

}

void
Waveform::append(uint32_t channel, double value)
{
    Q_ASSERT(channel < 2);
    Q_ASSERT(index[channel] < 7000);
    data[channel][index[channel]++] = value;
}

void
Waveform::setSamplerate(uint32_t samplerate)
{
    m_samplerate = samplerate;
}

uint32_t
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
    m_view = view;
}

uint32_t
Waveform::view()
{
    return m_view;
}

uint32_t
Waveform::getLength(uint32_t channel)
{
    Q_ASSERT(channel < 2);
    return index[channel];
}

double
Waveform::getX(uint32_t i)
{
    return getMinX() + i;
}

double
Waveform::getY(uint32_t i, uint32_t channel)
{
    Q_ASSERT(channel < 2);
    Q_ASSERT((m_trigger + i) < 7000);
    return data[channel][m_trigger+i];
}

double
Waveform::getMinX()
{
    return (double) m_view / -2;
}

double
Waveform::getMaxX()
{
    return (double) m_view / 2;
}

double
Waveform::getMinY(uint32_t channel)
{
    return -2;
}

double
Waveform::getMaxY(uint32_t channel)
{
    return 2;
}

} // namespace model
