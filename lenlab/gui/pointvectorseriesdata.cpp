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

#include "pointvectorseriesdata.h"

namespace gui {

PointVectorSeriesData::PointVectorSeriesData(model::MinMaxVector *time, model::MinMaxVector *value) :
    time(time),
    value(value)
{

}

QRectF
PointVectorSeriesData::boundingRect() const
{
    return QRectF(QPointF(time->getMin(), value->getMin()), QPointF(time->getMax(), value->getMax()));
}

size_t
PointVectorSeriesData::size() const
{
    return time->size();
}

QPointF
PointVectorSeriesData::sample( size_t i ) const
{
    return QPointF((*time)[i], (*value)[i]);
}

const QVector<double> &
PointVectorSeriesData::xData() const
{
    return *time;
}

const QVector<double> &
PointVectorSeriesData::yData() const
{
    return *value;
}

} // namespace gui
