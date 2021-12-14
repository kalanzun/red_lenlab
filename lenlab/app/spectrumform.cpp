#include "spectrumform.h"
#include "ui_spectrumform.h"

#include <QtCharts/QLogValueAxis>
#include <QtCharts/QValueAxis>

#include "model/lenlab.h"

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

    setupChart();
}

void SpectrumForm::setupChart() const
{
    ui->labChart->setXLabel("Frequenz [Hz]");
    ui->labChart->setYLabel("Amplitude [dB]");
    ui->labChart->setY2Label("Phase [°]");

    auto xAxis = new QLogValueAxis;
    // Note: With kHz, from 0.1 kHz it has rounding issues and the max tick is missing
    xAxis->setBase(10);
    xAxis->setLabelFormat("%g");
    xAxis->setMinorTickCount(-1); // automatic
    ui->labChart->addAxis(xAxis, Qt::AlignBottom);

    auto mAxis = new QValueAxis;
    mAxis->setTickCount(7); // 6 intervals
    mAxis->setMinorTickCount(4); // 5 intervals in each major interval
    ui->labChart->addAxis(mAxis, Qt::AlignLeft);

    auto phAxis = new QValueAxis;
    phAxis->setTickCount(7); // 6 intervals
    phAxis->setMinorTickCount(4); // 5 intervals in each major interval
    ui->labChart->addAxis(phAxis, Qt::AlignRight);

    ui->labChart->createChannel("Amplitude", true, xAxis, mAxis);
    ui->labChart->createChannel("Phase", true, xAxis, phAxis);
}

} // namespace app
