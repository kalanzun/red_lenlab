#include "loggerform.h"
#include "app/ui_loggerform.h"

#include "model/lenlab.h"
#include "labchart.h"

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
}

void LoggerForm::setupChart(LabChart* lab_chart) const
{
    lab_chart->setXLabel("Zeit [s]");
    lab_chart->setYLabel("Spannung [V]");

    lab_chart->createChannel("Kanal 1");
    lab_chart->createChannel("Kanal 2", false);
    lab_chart->createChannel("Kanal 3", false);
    lab_chart->createChannel("Kanal 4", false);

    lab_chart->createDefaultAxes();
}

} // namespace app
