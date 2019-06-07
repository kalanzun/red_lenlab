/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
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

#include "signalgenerator.h"

#include "lenlab.h"
#include "utils.h"

#include <QDebug>

#include <cmath>

namespace model {

double const Signalgenerator::OVERHEAD = 16.0 / 17.5;
uint32_t const Signalgenerator::CHANNELS = 2;
uint32_t const Signalgenerator::SAMPLES = 500;
uint32_t const Signalgenerator::BITS_PER_SAMPLE = 16;
uint32_t const Signalgenerator::SYSCLK = 80000000;
double const Signalgenerator::BASE_FREQUENCY = static_cast<double>(SYSCLK) / static_cast<double>(CHANNELS) / static_cast<double>(SAMPLES) / static_cast<double>(BITS_PER_SAMPLE) * OVERHEAD;

std::array< std::array< uint8_t const, 3 > const, 130 > const Signalgenerator::m_sine = {{
    {1, 2, 111}, // 0 20.59
    {1, 2, 106}, // 1 21.56
    {1, 2, 101}, // 2 22.63
    {1, 2, 96}, // 3 23.81
    {1, 2, 92}, // 4 24.84
    {1, 2, 88}, // 5 25.97
    {1, 2, 83}, // 6 27.54
    {1, 2, 80}, // 7 28.57
    {1, 2, 76}, // 8 30.08
    {1, 2, 72}, // 9 31.75
    {1, 2, 69}, // 10 33.13
    {1, 2, 66}, // 11 34.63
    {1, 2, 63}, // 12 36.28
    {1, 2, 60}, // 13 38.1
    {1, 2, 57}, // 14 40.1
    {1, 2, 54}, // 15 42.33
    {1, 2, 52}, // 16 43.96
    {1, 2, 49}, // 17 46.65
    {1, 2, 47}, // 18 48.63
    {1, 2, 45}, // 19 50.79
    {1, 2, 43}, // 20 53.16
    {1, 2, 41}, // 21 55.75
    {1, 2, 39}, // 22 58.61
    {1, 2, 37}, // 23 61.78
    {1, 2, 35}, // 24 65.31
    {1, 2, 34}, // 25 67.23
    {1, 2, 32}, // 26 71.43
    {1, 2, 30}, // 27 76.19
    {1, 2, 29}, // 28 78.82
    {1, 2, 28}, // 29 81.63
    {1, 2, 26}, // 30 87.91
    {1, 2, 25}, // 31 91.43
    {1, 2, 24}, // 32 95.24
    {1, 2, 23}, // 33 99.38
    {1, 2, 22}, // 34 103.9
    {1, 2, 21}, // 35 108.84
    {1, 2, 20}, // 36 114.29
    {1, 2, 19}, // 37 120.3
    {1, 2, 18}, // 38 126.98
    {1, 2, 17}, // 39 134.45
    {1, 2, 16}, // 40 142.86
    {2, 2, 31}, // 41 147.47
    {1, 2, 15}, // 42 152.38
    {1, 2, 14}, // 43 163.27
    {2, 2, 27}, // 44 169.31
    {1, 2, 13}, // 45 175.82
    {1, 2, 12}, // 46 190.48
    {2, 2, 23}, // 47 198.76
    {1, 2, 11}, // 48 207.79
    {2, 2, 21}, // 49 217.69
    {1, 2, 10}, // 50 228.57
    {2, 2, 19}, // 51 240.6
    {1, 2, 9}, // 52 253.97
    {3, 2, 26}, // 53 263.74
    {2, 2, 17}, // 54 268.91
    {1, 2, 8}, // 55 285.71
    {2, 2, 15}, // 56 304.76
    {3, 2, 22}, // 57 311.69
    {1, 2, 7}, // 58 326.53
    {2, 2, 13}, // 59 351.65
    {3, 2, 19}, // 60 360.9
    {1, 2, 6}, // 61 380.95
    {3, 2, 17}, // 62 403.36
    {2, 2, 11}, // 63 415.58
    {4, 2, 21}, // 64 435.37
    {1, 2, 5}, // 65 457.14
    {3, 2, 14}, // 66 489.8
    {2, 2, 9}, // 67 507.94
    {3, 2, 13}, // 68 527.47
    {1, 2, 4}, // 69 571.43
    {5, 2, 19}, // 70 601.5
    {3, 2, 11}, // 71 623.38
    {2, 2, 7}, // 72 653.06
    {3, 2, 10}, // 73 685.71
    {4, 2, 13}, // 74 703.3
    {1, 2, 3}, // 75 761.9
    {7, 2, 20}, // 76 800.0
    {4, 2, 11}, // 77 831.17
    {3, 2, 8}, // 78 857.14
    {2, 2, 5}, // 79 914.29
    {5, 2, 12}, // 80 952.38
    {3, 2, 7}, // 81 979.59
    {5, 2, 11}, // 82 1038.96
    {8, 2, 17}, // 83 1075.63
    {1, 2, 2}, // 84 1142.86
    {7, 2, 13}, // 85 1230.77
    {5, 2, 9}, // 86 1269.84
    {4, 2, 7}, // 87 1306.12
    {3, 2, 5}, // 88 1371.43
    {7, 2, 11}, // 89 1454.55
    {2, 2, 3}, // 90 1523.81
    {5, 2, 7}, // 91 1632.65
    {3, 2, 4}, // 92 1714.29
    {7, 2, 9}, // 93 1777.78
    {4, 2, 5}, // 94 1828.57
    {6, 2, 7}, // 95 1959.18
    {8, 2, 9}, // 96 2031.75
    {12, 2, 13}, // 97 2109.89
    {2, 2, 2}, // 98 2285.71
    {17, 2, 16}, // 99 2428.57
    {10, 2, 9}, // 100 2539.68
    {7, 2, 6}, // 101 2666.67
    {6, 2, 5}, // 102 2742.86
    {5, 2, 4}, // 103 2857.14
    {4, 2, 3}, // 104 3047.62
    {7, 2, 5}, // 105 3200.0
    {10, 2, 7}, // 106 3265.31
    {3, 2, 2}, // 107 3428.57
    {8, 2, 5}, // 108 3657.14
    {5, 2, 3}, // 109 3809.52
    {7, 2, 4}, // 110 4000.0
    {11, 2, 6}, // 111 4190.48
    {4, 2, 2}, // 112 4571.43
    {13, 2, 6}, // 113 4952.38
    {9, 2, 4}, // 114 5142.86
    {7, 2, 3}, // 115 5333.33
    {5, 2, 2}, // 116 5714.29
    {13, 2, 5}, // 117 5942.86
    {8, 2, 3}, // 118 6095.24
    {14, 2, 5}, // 119 6400.0
    {6, 2, 2}, // 120 6857.14
    {16, 2, 5}, // 121 7314.29
    {10, 2, 3}, // 122 7619.05
    {7, 2, 2}, // 123 8000.0
    {11, 2, 3}, // 124 8380.95
    {15, 2, 4}, // 125 8571.43
    {8, 2, 2}, // 126 9142.86
    {17, 2, 4}, // 127 9714.29
    {13, 2, 3}, // 128 9904.76
    {9, 2, 2}, // 129 10285.71
}};

Signalgenerator::Signalgenerator(Lenlab & lenlab, protocol::Board & board)
    : Component(lenlab, board)
    , m_amplitudeIndex(18)
    , m_frequencyIndex(m_sine.size())
    , m_secondIndex(21)
{
    double value;

    for (uint32_t i = 0; i < m_amplitudeIndex.length; ++i) {
        value = getAmplitude(i);
        m_amplitudeIndex.labels << QString("%1 V").arg(german_double(value));
    }

    for (uint32_t i = 0; i < m_frequencyIndex.length; ++i) {
        value = getFrequency(i);
        if (value < 100)
            m_frequencyIndex.labels << QString("%1 Hz").arg(german_double(std::round(10*value)/10));
        else if (value < 1000)
            m_frequencyIndex.labels << QString("%1 Hz").arg(german_double(std::round(value)));
        else if (value < 10000)
            m_frequencyIndex.labels << QString("%1 kHz").arg(german_double(std::round(value/10)/100));
        else
            m_frequencyIndex.labels << QString("%1 kHz").arg(german_double(std::round(value/100)/10));
    }

    for (uint32_t i = 0; i < m_secondIndex.length; ++i) {
        m_secondIndex.labels << QString("%1").arg(i);
    }
}

double
Signalgenerator::getAmplitude(uint32_t index)
{
    return (0.8 + 0.05 * index);
}

double
Signalgenerator::getFrequency(uint32_t index)
{
    return BASE_FREQUENCY * static_cast<double>(m_sine[index][0]) / static_cast<double>(m_sine[index][1] * m_sine[index][2]);
}

bool
Signalgenerator::locked() const
{
    return m_locked;
}

void
Signalgenerator::lock()
{
    Q_ASSERT(!m_locked);
    m_locked = true;
    emit lockedChanged(m_locked);
}

void
Signalgenerator::unlock()
{
    Q_ASSERT(m_locked);
    m_locked = false;
    emit lockedChanged(m_locked);
}

void
Signalgenerator::setAmplitude(uint32_t index)
{
    Q_ASSERT(index < m_amplitudeIndex.length);
    m_amplitude = index;
}

void
Signalgenerator::setFrequency(uint32_t index)
{
    Q_ASSERT(index < m_frequencyIndex.length);
    m_frequency = index;
}

void
Signalgenerator::setSecond(uint32_t index)
{
    Q_ASSERT(index < m_secondIndex.length);
    m_second = index;
}

IndexParameter const &
Signalgenerator::getAmplitudeIndex() const
{
    return m_amplitudeIndex;
}

IndexParameter const &
Signalgenerator::getFrequencyIndex() const
{
    return m_frequencyIndex;
}

IndexParameter const &
Signalgenerator::getSecondIndex() const
{
    return m_secondIndex;
}

void Signalgenerator::start()
{
    Q_ASSERT_X(!mActive, "Signalgenerator::start()", "Der Signalgenerator ist bereits aktiv.");
    setActive(true);

    setSine();
}

void Signalgenerator::stop()
{
    Q_ASSERT_X(mActive, "Signalgenerator::stop()", "Der Signalgenerator war nicht aktiv.");

    if (!mBoard.isOpen()) {
        return;
    }

    protocol::pTask task(new protocol::Task(::stopSignal));
    connect(task.data(), &protocol::Task::succeeded,
            this, &Signalgenerator::on_stop_succeeded);
    connect(task.data(), &protocol::Task::failed,
            this, &Signalgenerator::on_stop_failed);
    mBoard.queueTask(task);
}

void Signalgenerator::reset()
{
    super::reset();

    m_locked = false;
}

void
Signalgenerator::setSine()
{
    if (!mActive) return;

    if (!mBoard.isOpen()) {
        return;
    }

    QVector<uint32_t> args;
    args.append(m_sine[m_frequency][0]); // multiplier
    args.append(m_sine[m_frequency][1]); // predivider
    args.append(m_sine[m_frequency][2]); // divider
    args.append(static_cast<uint32_t>(std::round((1<<11) * getAmplitude(m_amplitude) / 1.65))); // amplitude
    args.append(m_second); // second

    protocol::pTask task(new protocol::Task(::setSignalSine));
    task->getCommand()->setUInt32Vector(args);
    connect(task.data(), &protocol::Task::succeeded,
            this, &Signalgenerator::on_set_sine_succeeded);
    connect(task.data(), &protocol::Task::failed,
            this, &Signalgenerator::on_set_sine_failed);
    mBoard.queueTask(task);
}

void Signalgenerator::on_set_sine_succeeded(protocol::pTask const & task)
{
    if (m_locked) emit succeeded(task);
}

void Signalgenerator::on_set_sine_failed(protocol::pTask const & task)
{
    if (m_locked) emit failed(task);
    else {
        emit mLenlab.logMessage(task->getErrorMessage());
        mLenlab.reset();
    }
}

void Signalgenerator::on_stop_succeeded(protocol::pTask const &)
{
    setActive(false);
}

void Signalgenerator::on_stop_failed(protocol::pTask const & task)
{
    emit mLenlab.logMessage(task->getErrorMessage());
    mLenlab.reset();
}

} // namespace model
