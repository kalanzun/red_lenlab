#ifndef POINTVECTORSERIESDATA_H
#define POINTVECTORSERIESDATA_H

#include "model/minmaxvector.h"
#include "qwt_series_data.h"


namespace gui {

class PointVectorSeriesData : public QwtSeriesData<QPointF>
{
public:
    PointVectorSeriesData(model::MinMaxVector *time, model::MinMaxVector *value);

    virtual QRectF boundingRect() const;

    virtual size_t size() const;
    virtual QPointF sample( size_t i ) const;

    const QVector<double> &xData() const;
    const QVector<double> &yData() const;

private:
    model::MinMaxVector *time;
    model::MinMaxVector *value;
};

} // namespace gui

#endif // POINTVECTORSERIESDATA_H
