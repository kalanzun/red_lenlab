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

#ifndef FREQUENCYSERIES_H
#define FREQUENCYSERIES_H

#include "series.h"

#include <QObject>

#include <array>

namespace model {

class FrequencySeries : public Series
{
    Q_OBJECT

    typedef Series super;

    static const int m_start_index = 34;
    static const int m_stop_index = 129;

    static const int m_channels = 2;
    static const int m_length = m_stop_index - m_start_index;

    std::array< int, m_channels > index;
    std::array< double, m_channels > MinY;
    std::array< double, m_channels > MaxY;
    std::array< std::array< double, m_length >, m_channels > data;

    int freq_index = 0;
    std::array< double, m_length > freq;

public:
    FrequencySeries();
    FrequencySeries(FrequencySeries const &) = delete;

    FrequencySeries & operator=(FrequencySeries const &) = delete;

    void append(int channel, double value);
    void appendFrequency(double value);

    int getChannels() const;
    int getLength() const;

    double getX(int i) const;
    double getY(int i, int channel) const;

    double getLastX() const;
    double getLastY(int channel) const;

    double getMinX() const;
    double getMaxX() const;
    double getMinY(int channel) const;
    double getMaxY(int channel) const;

    int startIndex() const;
    int stopIndex() const;
};

} // namespace model

#endif // FREQUENCYSERIES_H
