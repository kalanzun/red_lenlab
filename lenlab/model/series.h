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

#ifndef SERIES_H
#define SERIES_H

#include <QObject>

namespace model {

class Series : public QObject
{
    Q_OBJECT

public:
    explicit Series(QObject *parent = nullptr);

    virtual void append(uint32_t channel, double value) = 0;

    virtual uint32_t getLength(uint32_t channel) = 0;

    virtual double getX(uint32_t i) = 0;
    virtual double getY(uint32_t i, uint32_t channel) = 0;

    virtual double getMinX() = 0;
    virtual double getMaxX() = 0;
    virtual double getMinY(uint32_t channel) = 0;
    virtual double getMaxY(uint32_t channel) = 0;

signals:

public slots:
};

} // namespace model

#endif // SERIES_H
