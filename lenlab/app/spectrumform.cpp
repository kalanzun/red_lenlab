#include "spectrumform.h"
#include "ui_spectrumform.h"

#include <QtCharts/QLogValueAxis>
#include <QtCharts/QValueAxis>

#include "model/lenlab.h"
#include "labchart.h"

namespace app {

SpectrumForm::SpectrumForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpectrumForm)
{
    ui->setupUi(this);
}

SpectrumForm::~SpectrumForm()
{
    delete ui;
}

void SpectrumForm::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;
}

void SpectrumForm::setupChart(LabChart* lab_chart) const
{
    lab_chart->setXLabel("Frequenz [Hz]");
    lab_chart->setYLabel("Amplitude [dB]");
    lab_chart->setY2Label("Phase [°]");

    auto xAxis = new QLogValueAxis();
    // Note: With kHz, from 0.1 kHz it has rounding issues and the max tick is missing
    xAxis->setBase(10);
    xAxis->setLabelFormat("%g");
    xAxis->setMinorTickCount(-1); // automatic
    lab_chart->addAxis(xAxis, Qt::AlignBottom);

    auto mAxis = new QValueAxis();
    mAxis->setTickCount(7); // 6 intervals
    mAxis->setMinorTickCount(4); // 5 intervals in each major interval
    lab_chart->addAxis(mAxis, Qt::AlignLeft);

    auto phAxis = new QValueAxis();
    phAxis->setTickCount(7); // 6 intervals
    phAxis->setMinorTickCount(4); // 5 intervals in each major interval
    lab_chart->addAxis(phAxis, Qt::AlignRight);

    lab_chart->createChannel("Amplitude", true, xAxis, mAxis);
    lab_chart->createChannel("Phase", true, xAxis, phAxis);
}

} // namespace app
