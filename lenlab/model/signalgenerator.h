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

#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include "component.h"
#include "indexparameter.h"

#include <QObject>
#include <QTimer>

namespace model {

class Lenlab;

class Signalgenerator : public Component
{
    Q_OBJECT

    typedef Component super;

    static double const OVERHEAD;
    static uint32_t const CHANNELS;
    static uint32_t const SAMPLES;
    static uint32_t const BITS_PER_SAMPLE;
    static uint32_t const SYSCLK;
    static double const BASE_FREQUENCY;

    static std::array< std::array< uint8_t const, 3 > const, 130 > const m_sine;

    static int const m_task_timeout = 100;

    Q_PROPERTY(bool locked READ locked NOTIFY lockedChanged)

    bool m_locked = false;

    uint32_t m_amplitude = 0;
    uint32_t m_frequency = 0;
    uint32_t m_second = 0;

    IndexParameter m_amplitudeIndex;
    IndexParameter m_frequencyIndex;
    IndexParameter m_secondIndex;

public:
    explicit Signalgenerator(Lenlab & lenlab, protocol::Board & board);
    Signalgenerator(Signalgenerator const &) = delete;

    Signalgenerator & operator=(Signalgenerator const &) = delete;

    static double getAmplitude(uint32_t index);
    static double getFrequency(uint32_t index);

    bool locked() const;
    void lock();
    void unlock();

    void setAmplitude(uint32_t index);
    void setFrequency(uint32_t index);
    void setSecond(uint32_t index);

    IndexParameter const & getAmplitudeIndex() const;
    IndexParameter const & getFrequencyIndex() const;
    IndexParameter const & getSecondIndex() const;

    virtual void start();
    virtual void stop();
    virtual void reset();

    void setSine();

signals:
    void lockedChanged(bool locked);

    void succeeded(protocol::pTask const &);
    void failed(protocol::pTask const &);

private slots:
    void on_set_sine_succeeded(protocol::pTask const &);
    void on_set_sine_failed(protocol::pTask const &);
    void on_stop_succeeded(protocol::pTask const &);
    void on_stop_failed(protocol::pTask const &);
};

} // namespace model

#endif // SIGNALGENERATOR_H
