#include "loggerform.h"
#include "app/ui_loggerform.h"

#include "model/lenlab.h"
#include "model/logger.h"
#include "model/waveform.h"

namespace app {

LoggerForm::LoggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerForm)
{
    ui->setupUi(this);

    setupChart();

    connect(ui->ch1CheckBox, &QCheckBox::toggled,
            ui->labChart, [this](bool checked) { ui->labChart->setVisible(0, checked); });
    connect(ui->ch2CheckBox, &QCheckBox::toggled,
            ui->labChart, [this](bool checked) { ui->labChart->setVisible(1, checked); });
    connect(ui->ch3CheckBox, &QCheckBox::toggled,
            ui->labChart, [this](bool checked) { ui->labChart->setVisible(2, checked); });
    connect(ui->ch4CheckBox, &QCheckBox::toggled,
            ui->labChart, [this](bool checked) { ui->labChart->setVisible(3, checked); });
}

LoggerForm::~LoggerForm()
{
    delete ui;
}

void LoggerForm::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;
    logger = lenlab->logger;

    connect(logger, &model::Logger::WaveformCreated,
            this, &LoggerForm::setWaveform);
    setWaveform(logger->getWaveform());

    ui->labChart->setModel(logger);

    for (auto&& item : logger->interval) ui->intervalBox->addItem(item);
    connect(ui->intervalBox, &QComboBox::activated,
            &logger->interval, &model::Parameter::setIndex);
    ui->intervalBox->setCurrentIndex(logger->interval.getIndex());
}

void LoggerForm::setupChart() const
{
    ui->labChart->setXLabel("Zeit [s]");
    ui->labChart->setYLabel("Spannung [V]");

    ui->labChart->createChannel("Kanal 1");
    ui->labChart->createChannel("Kanal 2", false);
    ui->labChart->createChannel("Kanal 3", false);
    ui->labChart->createChannel("Kanal 4", false);

    ui->labChart->createDefaultAxes();
}

void LoggerForm::on_startButton_clicked()
{
    logger->start();
}

void LoggerForm::on_stopButton_clicked()
{
    logger->stop();
}

void LoggerForm::on_resetButton_clicked()
{
    logger->reset();
}

void LoggerForm::setWaveform(model::Waveform* waveform)
{
    ui->intervalBox->setEnabled(true);

    connect(waveform, &model::Waveform::Locked,
            ui->intervalBox, [this]() { ui->intervalBox->setEnabled(false); });
}

} // namespace app
