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

#ifndef SERIES_H
#define SERIES_H

#include <QObject>
#include <QSharedPointer>

namespace model {

class Series : public QObject
{
    Q_OBJECT

public:
    explicit Series(QObject *parent = nullptr);
    Series(Series const &) = delete;

    Series & operator=(Series const &) = delete;

    virtual void append(int channel, double value) = 0;

    virtual int getChannels() const = 0;
    virtual int getLength() const = 0;

    virtual double getX(int i) const = 0;
    virtual double getY(int i, int channel) const = 0;

    virtual double getLastX() const;
    virtual double getLastY(int channel) const;

    virtual double getMinX() const = 0;
    virtual double getMaxX() const = 0;
    virtual double getMinY(int channel) const = 0;
    virtual double getMaxY(int channel) const = 0;
};

typedef QSharedPointer<Series> pSeries;

} // namespace model

#endif // SERIES_H
