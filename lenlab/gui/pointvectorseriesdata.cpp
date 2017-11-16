#include "pointvectorseriesdata.h"

namespace gui {

PointVectorSeriesData::PointVectorSeriesData(model::MinMaxVector *time, model::MinMaxVector *value) : time(time), value(value)
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
