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

#ifndef FREQUENCYSERIES_H
#define FREQUENCYSERIES_H

#include "series.h"
#include <QObject>

namespace model {

class FrequencySeries : public Series
{
    Q_OBJECT

public:
    FrequencySeries();

    void append(uint32_t channel, double value);
    void clear();

    uint32_t getLength(uint32_t channel);

    double getX(uint32_t i);
    double getY(uint32_t i, uint32_t channel);

    double getMinX();
    double getMaxX();
    double getMinY(uint32_t channel);
    double getMaxY(uint32_t channel);

private:
    typedef Series super;

    std::array< uint32_t, 2 > index{{0, 0}};
    std::array< double, 2 > MinY{{0.1, -90}};
    std::array< double, 2 > MaxY{{10.0, 90}};
    std::array< std::array< double, 100 >, 2 > data;

};

} // namespace model

#endif // FREQUENCYSERIES_H
