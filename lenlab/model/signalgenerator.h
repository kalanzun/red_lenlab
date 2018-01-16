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

    uint8_t sine[100][3] = {
        {3, 14, 49}, // 19.99
        {2, 2, 213}, // 21.46
        {2, 2, 199}, // 22.97
        {3, 4, 139}, // 24.67
        {2, 2, 173}, // 26.42
        {3, 2, 242}, // 28.34
        {1, 2, 75}, // 30.48
        {1, 2, 70}, // 32.65
        {2, 2, 131}, // 34.9
        {1, 2, 61}, // 37.47
        {1, 2, 57}, // 40.1
        {1, 2, 53}, // 43.13
        {2, 2, 99}, // 46.18
        {1, 2, 46}, // 49.69
        {1, 2, 43}, // 53.16
        {1, 2, 40}, // 57.14
        {2, 2, 75}, // 60.95
        {1, 2, 35}, // 65.31
        {2, 2, 65}, // 70.33
        {3, 2, 91}, // 75.35
        {3, 2, 85}, // 80.67
        {5, 2, 132}, // 86.58
        {3, 2, 74}, // 92.66
        {1, 2, 23}, // 99.38
        {5, 2, 107}, // 106.81
        {1, 2, 20}, // 114.29
        {3, 2, 56}, // 122.45
        {3, 2, 52}, // 131.87
        {5, 2, 81}, // 141.09
        {1, 2, 15}, // 152.38
        {1, 2, 14}, // 163.27
        {7, 2, 92}, // 173.91
        {4, 2, 49}, // 186.59
        {7, 2, 80}, // 200.0
        {3, 2, 32}, // 214.29
        {1, 2, 10}, // 228.57
        {4, 2, 37}, // 247.1
        {3, 2, 26}, // 263.74
        {1, 2, 8}, // 285.71
        {2, 2, 15}, // 304.76
        {1, 2, 7}, // 326.53
        {2, 2, 13}, // 351.65
        {10, 2, 61}, // 374.71
        {3, 2, 17}, // 403.36
        {3, 2, 16}, // 428.57
        {1, 2, 5}, // 457.14
        {5, 2, 23}, // 496.89
        {3, 2, 13}, // 527.47
        {1, 2, 4}, // 571.43
        {4, 2, 15}, // 609.52
        {2, 2, 7}, // 653.06
        {4, 2, 13}, // 703.3
        {1, 2, 3}, // 761.9
        {6, 2, 17}, // 806.72
        {11, 2, 29}, // 867.0
        {13, 2, 32}, // 928.57
        {7, 2, 16}, // 1000.0
        {7, 2, 15}, // 1066.67
        {1, 2, 2}, // 1142.86
        {7, 2, 13}, // 1230.77
        {4, 2, 7}, // 1306.12
        {8, 2, 13}, // 1406.59
        {2, 2, 3}, // 1523.81
        {5, 2, 7}, // 1632.65
        {16, 2, 21}, // 1741.5
        {9, 2, 11}, // 1870.13
        {7, 2, 8}, // 2000.0
        {15, 2, 16}, // 2142.86
        {2, 2, 2}, // 2285.71
        {14, 2, 13}, // 2461.54
        {7, 2, 6}, // 2666.67
        {5, 2, 4}, // 2857.14
        {4, 2, 3}, // 3047.62
        {10, 2, 7}, // 3265.31
        {3, 2, 2}, // 3428.57
        {18, 2, 11}, // 3740.26
        {7, 2, 4}, // 4000.0
        {17, 2, 9}, // 4317.46
        {4, 2, 2}, // 4571.43
        {13, 2, 6}, // 4952.38
        {7, 2, 3}, // 5333.33
        {5, 2, 2}, // 5714.29
        {8, 2, 3}, // 6095.24
        {17, 2, 6}, // 6476.19
        {6, 2, 2}, // 6857.14
        {10, 2, 3}, // 7619.05
        {7, 2, 2}, // 8000.0
        {15, 2, 4}, // 8571.43
        {8, 2, 2}, // 9142.86
        {13, 2, 3}, // 9904.76
        {14, 2, 3}, // 10666.67
        {10, 2, 2}, // 11428.57
        {16, 2, 3}, // 12190.48
        {17, 2, 3}, // 12952.38
        {12, 2, 2}, // 13714.29
        {13, 2, 2}, // 14857.14
        {14, 2, 2}, // 16000.0
        {15, 2, 2}, // 17142.86
        {16, 2, 2}, // 18285.71
        {18, 2, 2}, // 20571.43
    };

public:
    explicit Signalgenerator(Lenlab *parent);

    double getFrequency(uint32_t index);

    void setSine();
    void try_to_setSine();

    void setAmplitude(uint32_t index);
    void setFrequency(uint32_t index);
    void setDivider(uint32_t index);

    IndexParameter amplitudeIndex;
    IndexParameter frequencyIndex;
    IndexParameter dividerIndex;

signals:

public slots:
    void on_reply(const pCommunication &com, const usb::pMessage &reply);

private:
    Lenlab *lenlab;

    uint32_t amplitude = 0;
    uint32_t frequency = 0;
    uint32_t second = 0;

    bool pending = 0;
};

} // namespace model

#endif // SIGNALGENERATOR_H
