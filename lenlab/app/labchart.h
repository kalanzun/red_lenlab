#ifndef APP_LABCHART_H
#define APP_LABCHART_H

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QWidget>

#include "model/lenlab.h"

namespace app {

namespace Ui {
class LabChart;
}

class LabChart : public QWidget
{
    Q_OBJECT

    QChart* chart;
    QVector< QLineSeries* > series;

    model::Lenlab* lenlab;

public:
    explicit LabChart(QWidget *parent = nullptr);
    ~LabChart();

    void setModel(model::Lenlab* lenlab);

    void createSeries(const QString& name);
    void createDefaultAxes();

    void setWaveform(QSharedPointer< model::Waveform >& waveform);

private:
    Ui::LabChart *ui;
};

} // namespace app
#endif // APP_LABCHART_H
