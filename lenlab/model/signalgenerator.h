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

#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include "component.h"
#include "indexparameter.h"
#include "communication.h"
#include <QObject>

namespace model {

/**
 * @brief Lenlab Signal Generator Component.
 */

class Signalgenerator : public QObject
{
    Q_OBJECT

    static const uint32_t sine_length = 130;

    uint8_t sine[sine_length][3] = {
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
    };

public:
    explicit Signalgenerator(Lenlab *parent);

    double getFrequency(uint32_t index);

    void lock();
    void unlock();
    bool locked();

    void setSine();
    void try_to_setSine();
    void setSine_cmd();

    void stop();
    void try_to_stop();
    void stop_cmd();

    void setAmplitude(uint32_t index);
    void setFrequency(uint32_t index);
    void setSecond(uint32_t index);

    IndexParameter amplitudeIndex;
    IndexParameter frequencyIndex;
    IndexParameter secondIndex;

signals:
    void updated();
    void lockedDataChanged(bool locked);

public slots:
    void on_reply(const pCommunication &com, const usb::pMessage &reply);

private:
    Lenlab *lenlab;

    bool m_locked;
    bool setSine_pending = 0;
    bool stop_pending = 0;

    uint32_t amplitude = 0;
    uint32_t frequency = 0;
    uint32_t second = 0;
};

} // namespace model

#endif // SIGNALGENERATOR_H
