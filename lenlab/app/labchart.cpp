#include "labchart.h"
#include "app/ui_labchart.h"

#include "model/component.h"
#include "model/waveform.h"

namespace app {

LabChart::LabChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LabChart)
{
    ui->setupUi(this);

    chart = ui->chart_view->chart();
}

LabChart::~LabChart()
{
    delete ui;
}

void LabChart::setModel(model::Component* component)
{
    this->component = component;

    for (auto name : component->channel_names()) {
        auto s = new QLineSeries();
        s->setName(name);
        series.append(s);
        chart->addSeries(s);
    }

    chart->createDefaultAxes();
}

void LabChart::setWaveform(model::Waveform* waveform)
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
