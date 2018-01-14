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

Waveform::Waveform(uint32_t channels, QObject *parent) : QObject(parent), data(QVector< QVector < double >>(channels))
{
}

void
Waveform::append(uint32_t channel, double value)
{
    data[channel].append(value);
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
    return m_view ? m_view : getLength();
}

uint32_t
Waveform::getLength()
{
    return data[0].size();
}

double
Waveform::getTime(uint32_t i)
{
    return m_view ? getMinTime() + i : i;
}

double
Waveform::getValue(uint32_t channel, uint32_t i)
{
    Q_ASSERT(channel < data.size());
    Q_ASSERT((m_trigger + i) < data[channel].size());
    return data[channel][m_trigger+i];
}

double
Waveform::getMinTime()
{
    return m_view ? (double) m_view / -2 : 0;
}

double
Waveform::getMaxTime()
{
    return m_view ? (double) m_view / 2 : getLength();
}

double
Waveform::getMinValue()
{
    return -2;
}

double
Waveform::getMaxValue()
{
    return 2;
}


} // namespace model
