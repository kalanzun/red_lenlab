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
#include <QDebug>

namespace gui {

PointVectorSeriesData::PointVectorSeriesData(model::pSeries const & series, std::size_t channel) :
    series(series),
    channel(channel)
{
}

QRectF
PointVectorSeriesData::boundingRect() const
{
    return QRectF(QPointF(series->getMinX(), series->getMinY(channel)), QPointF(series->getMaxX(), series->getMaxY(channel)));
}

size_t
PointVectorSeriesData::size() const
{
    return series->getLength(channel);
}

QPointF
PointVectorSeriesData::sample( size_t i ) const
{
    return QPointF(series->getX(i), series->getY(i, channel));
}

} // namespace gui
