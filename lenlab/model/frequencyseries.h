/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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

    static const std::size_t m_start_index = 34;
    static const std::size_t m_stop_index = 129;

    static const std::size_t m_channels = 3;
    static const std::size_t m_length = m_stop_index - m_start_index;

    std::array< std::size_t, m_channels > index;
    std::array< double, m_channels > MinY;
    std::array< double, m_channels > MaxY;
    std::array< std::array< double, m_length >, m_channels > data;

public:
    FrequencySeries();
    FrequencySeries(FrequencySeries const &) = delete;

    FrequencySeries & operator=(FrequencySeries const &) = delete;

    void append(std::size_t channel, double value);

    std::size_t getChannels() const;
    std::size_t getLength() const;

    double getX(std::size_t i) const;
    double getY(std::size_t i, std::size_t channel) const;

    double getLastX() const;
    double getLastY(std::size_t channel) const;

    double getMinX() const;
    double getMaxX() const;
    double getMinY(std::size_t channel) const;
    double getMaxY(std::size_t channel) const;

    std::size_t startIndex() const;
    std::size_t stopIndex() const;
};

} // namespace model

#endif // FREQUENCYSERIES_H
