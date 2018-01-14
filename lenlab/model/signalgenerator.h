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
#include <QObject>

namespace model {

/**
 * @brief Lenlab Signal Generator Component.
 */

class Signalgenerator : public Component
{
    Q_OBJECT

    uint8_t sine[100][3] = {
        {1, 2, 125}, // 20.0
        {1, 2, 117}, // 21.37
        {1, 2, 109}, // 22.94
        {1, 2, 101}, // 24.75
        {1, 2, 95}, // 26.32
        {1, 2, 88}, // 28.41
        {1, 2, 82}, // 30.49
        {1, 2, 77}, // 32.47
        {1, 2, 72}, // 34.72
        {1, 2, 67}, // 37.31
        {1, 2, 62}, // 40.32
        {1, 2, 58}, // 43.1
        {1, 2, 54}, // 46.3
        {1, 2, 50}, // 50.0
        {1, 2, 47}, // 53.19
        {1, 2, 44}, // 56.82
        {1, 2, 41}, // 60.98
        {1, 2, 38}, // 65.79
        {1, 2, 36}, // 69.44
        {1, 2, 33}, // 75.76
        {1, 2, 31}, // 80.65
        {1, 2, 29}, // 86.21
        {1, 2, 27}, // 92.59
        {1, 2, 25}, // 100.0
        {2, 2, 47}, // 106.38
        {1, 2, 22}, // 113.64
        {2, 2, 41}, // 121.95
        {1, 2, 19}, // 131.58
        {1, 2, 18}, // 138.89
        {2, 2, 33}, // 151.52
        {2, 2, 31}, // 161.29
        {2, 2, 29}, // 172.41
        {2, 2, 27}, // 185.19
        {2, 2, 25}, // 200.0
        {3, 2, 35}, // 214.29
        {1, 2, 11}, // 227.27
        {1, 2, 10}, // 250.0
        {2, 2, 19}, // 263.16
        {1, 2, 9}, // 277.78
        {1, 2, 8}, // 312.5
        {3, 2, 23}, // 326.09
        {1, 2, 7}, // 357.14
        {3, 2, 20}, // 375.0
        {4, 2, 25}, // 400.0
        {1, 2, 6}, // 416.67
        {2, 2, 11}, // 454.55
        {1, 2, 5}, // 500.0
        {3, 2, 14}, // 535.71
        {3, 2, 13}, // 576.92
        {1, 2, 4}, // 625.0
        {5, 2, 19}, // 657.89
        {2, 2, 7}, // 714.29
        {3, 2, 10}, // 750.0
        {1, 2, 3}, // 833.33
        {6, 2, 17}, // 882.35
        {3, 2, 8}, // 937.5
        {2, 2, 5}, // 1000.0
        {3, 2, 7}, // 1071.43
        {5, 2, 11}, // 1136.36
        {1, 2, 2}, // 1250.0
        {7, 2, 13}, // 1346.15
        {4, 2, 7}, // 1428.57
        {3, 2, 5}, // 1500.0
        {2, 2, 3}, // 1666.67
        {5, 2, 7}, // 1785.71
        {3, 2, 4}, // 1875.0
        {4, 2, 5}, // 2000.0
        {6, 2, 7}, // 2142.86
        {10, 2, 11}, // 2272.73
        {2, 2, 2}, // 2500.0
        {12, 2, 11}, // 2727.27
        {8, 2, 7}, // 2857.14
        {6, 2, 5}, // 3000.0
        {4, 2, 3}, // 3333.33
        {7, 2, 5}, // 3500.0
        {3, 2, 2}, // 3750.0
        {8, 2, 5}, // 4000.0
        {5, 2, 3}, // 4166.67
        {9, 2, 5}, // 4500.0
        {4, 2, 2}, // 5000.0
        {11, 2, 5}, // 5500.0
        {7, 2, 3}, // 5833.33
        {5, 2, 2}, // 6250.0
        {8, 2, 3}, // 6666.67
        {11, 2, 4}, // 6875.0
        {6, 2, 2}, // 7500.0
        {10, 2, 3}, // 8333.33
        {7, 2, 2}, // 8750.0
        {11, 2, 3}, // 9166.67
        {8, 2, 2}, // 10000.0
        {13, 2, 3}, // 10833.33
        {9, 2, 2}, // 11250.0
        {10, 2, 2}, // 12500.0
        {16, 2, 3}, // 13333.33
        {11, 2, 2}, // 13750.0
        {12, 2, 2}, // 15000.0
        {13, 2, 2}, // 16250.0
        {14, 2, 2}, // 17500.0
        {15, 2, 2}, // 18750.0
        {16, 2, 2}, // 20000.0
    };

public:
    explicit Signalgenerator(Lenlab *parent);

    void setSine(uint32_t index);

    double getFrequency(uint32_t index);

signals:

public slots:

private:
    typedef Component super;
};

} // namespace model

#endif // SIGNALGENERATOR_H
