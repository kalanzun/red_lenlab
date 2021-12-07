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

    chart->legend()->hide();
}

LabChart::~LabChart()
{
    delete ui;
}

void LabChart::setModel(model::Component* component)
{
    this->component = component;

    ui->x_label->setText(component->x_label());
    ui->y_label->setText(component->y_label());

    auto y2_label = component->y2_label();
    if (y2_label.isEmpty()) {
        ui->y2_label->hide();
    }
    else {
        ui->y2_label->setText(y2_label);
    }

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
