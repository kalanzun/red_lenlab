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
#include "utils.h"
#include <cmath>
#include <QDebug>

namespace model {

Signalgenerator::Signalgenerator(Lenlab & lenlab, protocol::Board & board)
    : QObject()
    , mLenlab(lenlab)
    , mBoard(board)
    , amplitudeIndex(18)
    , frequencyIndex(sine_length)
    , secondIndex(21)
{
    double value;

    for (uint32_t i = 0; i < amplitudeIndex.length; ++i) {
        value = 0.8 + 0.05 * i;
        amplitudeIndex.labels << QString("%1 V").arg(german_double(value));
    }

    for (uint32_t i = 0; i < frequencyIndex.length; ++i) {
        value = getFrequency(i);
        if (value < 100)
            frequencyIndex.labels << QString("%1 Hz").arg(german_double(std::round(10*value)/10));
        else if (value < 1000)
            frequencyIndex.labels << QString("%1 Hz").arg(german_double(std::round(value)));
        else if (value < 10000)
            frequencyIndex.labels << QString("%1 kHz").arg(german_double(std::round(value/10)/100));
        else
            frequencyIndex.labels << QString("%1 kHz").arg(german_double(std::round(value/100)/10));
    }

    for (uint32_t i = 0; i < secondIndex.length; i++) {
        secondIndex.labels << QString("%1").arg(i);
    }
}

// TODO static members
#define OVERHEAD (16.0/17.5)
#define CHANNELS (2)
#define SAMPLES (500)
#define BITS_PER_SAMPLE (16)
#define SYSCLK (80000000.0)
#define BASE_FREQUENCY (SYSCLK / CHANNELS / SAMPLES / BITS_PER_SAMPLE * OVERHEAD)

double
Signalgenerator::getFrequency(uint32_t index)
{
    Q_ASSERT(index < frequencyIndex.length);
    return BASE_FREQUENCY  * (double) sine[index][0] / (double) (sine[index][1] * sine[index][2]);
}

void
Signalgenerator::lock()
{
    Q_ASSERT(!m_locked);
    m_locked = 1;
    emit lockedDataChanged(1);
}

void
Signalgenerator::unlock()
{
    Q_ASSERT(m_locked);
    m_locked = 0;
    emit lockedDataChanged(0);
}

bool
Signalgenerator::locked()
{
    return m_locked;
}

void
Signalgenerator::setSine()
{
    setSine_pending = 1;
    try_to_setSine();
}

void
Signalgenerator::try_to_setSine() {
    /*
    if (setSine_pending && lenlab->available()) {
        setSine_cmd();
        setSine_pending = 0;
    }
    */
}

void
Signalgenerator::setSine_cmd()
{
    /*
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
    */
}

void
Signalgenerator::stop()
{
    stop_pending = 1;
    try_to_stop();
}

void
Signalgenerator::try_to_stop() {
    /*
    if (stop_pending && lenlab->available()) {
        stop_cmd();
        stop_pending = 0;
    }
    */
}

void
Signalgenerator::stop_cmd()
{
    /*
    auto com = lenlab->initCommunication();
    connect(com, SIGNAL(reply(pCommunication, usb::pMessage)),
            this, SLOT(on_reply(pCommunication, usb::pMessage)));
    auto cmd = usb::newCommand(stopSignal);
    cmd->setBodyLength(0);
    com->send(cmd);
    com->deleteLater();
    */
}
/*
void
Signalgenerator::on_reply(const pCommunication &com, const usb::pMessage &reply)
{
    Q_UNUSED(reply);
    com->deleteLater();

    emit updated();
}
*/
void
Signalgenerator::setAmplitude(uint32_t index)
{
    Q_ASSERT(index < amplitudeIndex.length);
    amplitude = index;
}

void
Signalgenerator::setFrequency(uint32_t index)
{
    Q_ASSERT(index < frequencyIndex.length);
    frequency = index;
}

void
Signalgenerator::setSecond(uint32_t index)
{
    Q_ASSERT(index < secondIndex.length);
    second = index;
}

} // namespace model
