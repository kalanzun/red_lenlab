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

#ifndef LOGGERSERIES_H
#define LOGGERSERIES_H

#include "series.h"

#include <QObject>
#include <QVector>

#include <array>

namespace model {

class Loggerseries : public Series
{
    Q_OBJECT

    typedef Series super;

    Q_PROPERTY(uint32_t interval READ interval WRITE setInterval)
    uint32_t m_interval = 0;

    std::array< QVector<double>, 4 > data;

public:
    explicit Loggerseries();
    Loggerseries(Loggerseries const &) = delete;

    Loggerseries & operator=(Loggerseries const &) = delete;

    void setInterval(uint32_t interval);
    uint32_t interval() const;

    void append(std::size_t channel, double value);

    std::size_t getChannels() const;
    std::size_t getLength(std::size_t channel) const;

    double getX(std::size_t i) const;
    double getY(std::size_t i, std::size_t channel) const;

    double getMinX() const;
    double getMaxX() const;
    double getMinY(std::size_t channel) const;
    double getMaxY(std::size_t channel) const;
};

} // namespace model

#endif // LOGGERSERIES_H
