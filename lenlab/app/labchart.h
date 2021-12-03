#ifndef APP_LABCHART_H
#define APP_LABCHART_H

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QObject>

#include "model/waveform.h"

namespace app {

class LabChart : public QObject
{
    Q_OBJECT
public:
    QChart* chart;
    QVector< QLineSeries* > series;

    QChartView* chart_view;

    explicit LabChart(QObject *parent = nullptr);

    void setChartView(QChartView* chart_view);

signals:

public slots:
    void setWaveform(QSharedPointer< model::Waveform >& waveform);

};

} // namespace app

#endif // APP_LABCHART_H
