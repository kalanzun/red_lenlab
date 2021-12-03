#include "labchart.h"

#include <QDebug>

namespace app {

LabChart::LabChart(QObject *parent)
    : QObject{parent}
{

}

void LabChart::setChartView(QChartView* chart_view)
{
    //chart = new QChart();

    this->chart_view = chart_view;
    this->chart = chart_view->chart();
    //this->chart_view->setChart(chart);

    for (int i = 0; i < model::Waveform::channels; ++i) {
        auto s = new QLineSeries();
        s->setName(QString("Kanal ") + QString::number(i + 1));
        series.append(s);
        chart->addSeries(s);
    }

    chart->createDefaultAxes();
}

void LabChart::setWaveform(QSharedPointer< model::Waveform >& waveform)
{
    qDebug() << "setWaveform";

    QVector< QPointF > list;
    list.reserve(waveform->getLength());

    for (int i = 0; i < waveform->channels && i < series.size(); ++i) {
        for (auto point : waveform->getChannel(i)) {
            list.append({point.x, point.y});
        }
        series.at(i)->replace(list);
        list.clear();
    }
}

} // namespace app
