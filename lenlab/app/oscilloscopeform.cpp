#include "oscilloscopeform.h"
#include "app/ui_oscilloscopeform.h"

#include "model/lenlab.h"
#include "model/oscilloscope.h"

namespace app {

OscilloscopeForm::OscilloscopeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OscilloscopeForm)
{
    ui->setupUi(this);

    connect(ui->ch1CheckBox, &QCheckBox::toggled,
            ui->labChart, [this](bool checked) { ui->labChart->setVisible(0, checked); });
    connect(ui->ch2CheckBox, &QCheckBox::toggled,
            ui->labChart, [this](bool checked) { ui->labChart->setVisible(1, checked); });
}

OscilloscopeForm::~OscilloscopeForm()
{
    delete ui;
}

void OscilloscopeForm::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;
    this->oscilloscope = lenlab->oscilloscope;

    ui->labChart->setModel(lenlab->oscilloscope);
    setupChart();

    for (auto&& item : oscilloscope->samplerate) ui->samplerateBox->addItem(item);
    connect(ui->samplerateBox, &QComboBox::activated,
            &oscilloscope->samplerate, &model::Parameter::setIndex);
    ui->samplerateBox->setCurrentIndex(oscilloscope->samplerate.getIndex());

    for (auto&& item : oscilloscope->timerange) ui->xRangeBox->addItem(item);
    connect(ui->xRangeBox, &QComboBox::activated,
            &oscilloscope->timerange, &model::Parameter::setIndex);
    ui->xRangeBox->setCurrentIndex(oscilloscope->timerange.getIndex());

    for (auto&& item : oscilloscope->valuerange) ui->yRangeBox->addItem(item);
    connect(ui->yRangeBox, &QComboBox::activated,
            &oscilloscope->valuerange, &model::Parameter::setIndex);
    ui->yRangeBox->setCurrentIndex(oscilloscope->valuerange.getIndex());
}

void OscilloscopeForm::setupChart() const
{
    ui->labChart->setXLabel("Zeit [ms]");
    ui->labChart->setYLabel("Spannung [V]");

    ui->labChart->createChannel("Kanal 1");
    ui->labChart->createChannel("Kanal 2", false);

    ui->labChart->createDefaultAxes();
}

void OscilloscopeForm::on_startButton_clicked()
{
    oscilloscope->start();
}

} // namespace app
