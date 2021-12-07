#ifndef APP_LABCHART_H
#define APP_LABCHART_H

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QWidget>

#include "model/component.h"
#include "model/waveform.h"

namespace app {

namespace Ui {
class LabChart;
}

class LabChart : public QWidget
{
    Q_OBJECT

    QChart* chart;
    QVector< QLineSeries* > series;

    model::Component* component;

public:
    explicit LabChart(QWidget *parent = nullptr);
    ~LabChart();

    void setModel(model::Component* component);

    void setWaveform(model::Waveform* waveform);

private:
    Ui::LabChart *ui;
};

} // namespace app
#endif // APP_LABCHART_H
