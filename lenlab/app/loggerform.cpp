#include "loggerform.h"
#include "app/ui_loggerform.h"

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

void LoggerForm::setupChart(LabChart* lab_chart)
{
    for (int i = 0; i < model::Waveform::channels; ++i) {
        lab_chart->createSeries(QString("Kanal ") + QString::number(i + 1));
    }

    lab_chart->createDefaultAxes();
}

} // namespace app
