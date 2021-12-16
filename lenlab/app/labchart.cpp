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
    ui->y2_label->hide();
}

LabChart::~LabChart()
{
    delete ui;
}

void LabChart::setXLabel(const QString& label)
{
    ui->x_label->setText(label);
}

void LabChart::setYLabel(const QString& label)
{
    ui->y_label->setText(label);
}

void LabChart::setY2Label(const QString& label)
{
    ui->y2_label->setText(label);
    ui->y2_label->show();
}

void LabChart::addAxis(QAbstractAxis* axis, Qt::Alignment alignment)
{
    chart->addAxis(axis, alignment);
}

void LabChart::createChannel(const QString& name, bool visible, QAbstractAxis* xAxis, QAbstractAxis* yAxis)
{
    auto s = new QLineSeries;
    s->setName(name);
    s->setVisible(visible);
    series.append(s);
    chart->addSeries(s);

    if (xAxis) s->attachAxis(xAxis);
    if (yAxis) s->attachAxis(yAxis);
}

void LabChart::createDefaultAxes()
{
    chart->createDefaultAxes();
}

void LabChart::setModel(model::Component* component)
{
    this->component = component;

    setWaveform(component->getWaveform());

    connect(component, &model::Component::WaveformCreated,
            this, &LabChart::setWaveform);
}

void LabChart::setWaveform(model::Waveform* waveform)
{
    qDebug() << "setWaveform";

    QVector< QPointF > list;
    list.reserve(waveform->getLength());

    for (auto i = 0; i < waveform->channels && i < series.size(); ++i) {
        for (auto point : waveform->getChannel(i)) {
            list.append({point.x, point.y});
        }
        series.at(i)->replace(list);
        list.clear();
    }

    setRange(waveform);

    connect(waveform, &model::Waveform::SampleAppended,
            this, &LabChart::appendSample);
}

void LabChart::setRange(model::Waveform* waveform)
{
    auto axes = chart->axes(Qt::Horizontal);
    if (!axes.isEmpty()) axes.first()->setRange(0, 5000);

    axes = chart->axes(Qt::Vertical);
    if (!axes.isEmpty()) axes.first()->setRange(-2, 2);
    if (axes.size() > 1) axes.at(1)->setRange(-180, 0);
}

void LabChart::setVisible(int channel, bool visible)
{
    series.at(channel)->setVisible(visible);
}

void LabChart::appendSample(const struct model::Sample& sample)
{
    qDebug() << "appendSample";

    for (auto i = 0; i < sample.channels && i < series.size(); ++i) {
        series.at(i)->append(sample.x, sample.y[i]);
    }
}

} // namespace app
