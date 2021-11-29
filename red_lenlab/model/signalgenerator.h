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

    int m_amplitude_index = 0;
    int m_frequency_index = 0;
    int m_second_index = 0;

public:
    static int const amplitude_count = 18;
    static int const frequency_count;
    static int const second_count = 21;

    explicit Signalgenerator(Lenlab & lenlab, protocol::Board & board);
    Signalgenerator(Signalgenerator const &) = delete;

    Signalgenerator & operator=(Signalgenerator const &) = delete;

    bool locked() const;
    void lock();
    void unlock();

    void setAmplitudeIndex(int index);
    void setFrequencyIndex(int index);
    void setSecondIndex(int index);

    static double to_amplitude(int index);
    static double to_frequency(int index);

    static QString getAmplitudeLabel(int index);
    static QString getFrequencyLabel(int index);
    static QString getSecondLabel(int index);

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
