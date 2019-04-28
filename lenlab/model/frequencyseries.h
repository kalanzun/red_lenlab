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

#include <array>

namespace model {

class FrequencySeries : public Series
{
    Q_OBJECT

public:
    FrequencySeries();

    static const uint32_t start_index = 34;
    static const uint32_t stop_index = 129;

    static const uint32_t channel_length = 3;
    static const uint32_t values_length = stop_index - start_index;

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

    std::array< uint32_t, channel_length > index;
    std::array< double, channel_length > MinY;
    std::array< double, channel_length > MaxY;
    std::array< std::array< double, values_length >, channel_length > data;

};

} // namespace model

#endif // FREQUENCYSERIES_H
