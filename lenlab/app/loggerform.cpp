#include "loggerform.h"
#include "app/ui_loggerform.h"

#include "model/lenlab.h"

namespace app {

LoggerForm::LoggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerForm)
{
    ui->setupUi(this);
}

LoggerForm::~LoggerForm()
{
    delete ui;
}

void LoggerForm::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;

    setupChart();
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

} // namespace app
