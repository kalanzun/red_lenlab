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

Waveform::Waveform(QObject *parent) : QObject(parent)
{
    i[0] = 0;
    i[1] = 0;
}

void
Waveform::append(uint32_t channel, uint16_t value)
{
    //Q_ASSERT(channel < data.size());
    //Q_ASSERT(i[channel] < 7000);
    data[channel][i[channel]++] = ((((double) value) / 1024.0 - 0.5) * 3.3);
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

uint32_t
Waveform::getDataLength()
{
    return data[0].size();
}

uint32_t
Waveform::getViewLength()
{
    return 6000;
}

double
Waveform::getTime(uint32_t i)
{
    return (double) i - 3000.0;
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
    return -3000;
}

double
Waveform::getMaxTime()
{
    return 3000;
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
