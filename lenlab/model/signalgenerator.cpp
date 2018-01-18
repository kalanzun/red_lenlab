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

#include "lenlab.h"
#include "signalgenerator.h"
#include <cmath>

namespace model {

Signalgenerator::Signalgenerator(Lenlab *parent) : QObject(), amplitudeIndex(18), frequencyIndex(100), dividerIndex(21), lenlab(parent)
{
    double value;

    for (uint32_t i = 0; i < amplitudeIndex.length; i++) {
        value = 0.8 + 0.05 * i;
        amplitudeIndex.labels << QString("%1 V").arg(value);
    }

    for (uint32_t i = 0; i < frequencyIndex.length; i++) {
        value = getFrequency(i);
        if (value < 100)
            frequencyIndex.labels << QString("%1 Hz").arg(std::round(10*value)/10);
        else if (value < 1000)
            frequencyIndex.labels << QString("%1 Hz").arg(std::round(value));
        else if (value < 10000)
            frequencyIndex.labels << QString("%1 kHz").arg(std::round(value/10)/100);
        else
            frequencyIndex.labels << QString("%1 kHz").arg(std::round(value/100)/10);
    }

    for (uint32_t i = 0; i < dividerIndex.length; i++) {
        dividerIndex.labels << QString("%1").arg(i);
    }
}

#define OVERHEAD (16.0/17.5)
#define CHANNELS (2)
#define SAMPLES (500)
#define BITS_PER_SAMPLE (16)
#define SYSCLK (80000000.0)
#define BASE_FREQUENCY (SYSCLK / CHANNELS / SAMPLES / BITS_PER_SAMPLE * OVERHEAD)

double
Signalgenerator::getFrequency(uint32_t index)
{
    return BASE_FREQUENCY  * (double) sine[index][0] / (double) (sine[index][1] * sine[index][2]);
}

void
Signalgenerator::setSine()
{
    auto com = lenlab->initCommunication();
    connect(com, SIGNAL(reply(pCommunication, usb::pMessage)),
            this, SLOT(on_reply(pCommunication, usb::pMessage)));
    auto cmd = usb::newCommand(setSignalSine);
    cmd->setBodyLength(0);
    cmd->setType(IntArray);
    cmd->setInt(sine[frequency][0]); // multiplier
    cmd->setInt(sine[frequency][1]); // predivider
    cmd->setInt(sine[frequency][2]); // divider
    cmd->setInt(std::round((1<<11) * (0.8 + 0.05 * amplitude) / 1.65)); // amplitude
    cmd->setInt(second); // second
    com->send(cmd);
}

void
Signalgenerator::try_to_setSine() {
    if (pending && lenlab->available()) {
        setSine();
        pending = 0;
    }
}

void
Signalgenerator::on_reply(const pCommunication &com, const usb::pMessage &reply)
{
    Q_UNUSED(reply);
    com->deleteLater();

    emit updated();
}

void
Signalgenerator::setAmplitude(uint32_t index)
{
    Q_ASSERT(index < amplitudeIndex.length);
    amplitude = index;
    pending = 1;
    try_to_setSine();
}

void
Signalgenerator::setFrequency(uint32_t index)
{
    Q_ASSERT(index < frequencyIndex.length);
    frequency = index;
    pending = 1;
    try_to_setSine();
}

void
Signalgenerator::setDivider(uint32_t index)
{
    Q_ASSERT(index < dividerIndex.length);
    second = index;
    pending = 1;
    try_to_setSine();
}

} // namespace model
