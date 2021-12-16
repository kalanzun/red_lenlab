#include "loggerform.h"
#include "app/ui_loggerform.h"

#include "model/lenlab.h"
#include "model/logger.h"

namespace app {

LoggerForm::LoggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerForm)
{
    ui->setupUi(this);

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
    this->logger = lenlab->logger;

    ui->labChart->setModel(lenlab->logger);
    setupChart();

    for (auto&& item : logger->interval) ui->intervalBox->addItem(item);
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

} // namespace app
