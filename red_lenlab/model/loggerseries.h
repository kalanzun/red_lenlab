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
    uint32_t m_interval = 1000;

    static const int m_channels = 4;

    std::array< QVector<double>, m_channels > data;

public:
    explicit Loggerseries();
    Loggerseries(Loggerseries const &) = delete;

    Loggerseries & operator=(Loggerseries const &) = delete;

    void setInterval(uint32_t interval);
    uint32_t interval() const;

    void append(int channel, double value);

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

private:
    static int round_up_towards(int base, double value);
};

} // namespace model

#endif // LOGGERSERIES_H
