#include "oscilloscopeform.h"
#include "app/ui_oscilloscopeform.h"

#include "model/lenlab.h"
#include "labchart.h"

namespace app {

OscilloscopeForm::OscilloscopeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OscilloscopeForm)
{
    ui->setupUi(this);
}

OscilloscopeForm::~OscilloscopeForm()
{
    delete ui;
}

void OscilloscopeForm::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;
}

void OscilloscopeForm::setupChart(LabChart* lab_chart) const
{
    lab_chart->setXLabel("Zeit [ms]");
    lab_chart->setYLabel("Spannung [V]");

    lab_chart->createChannel("Kanal 1");
    lab_chart->createChannel("Kanal 2", false);

    lab_chart->createDefaultAxes();
}

} // namespace app
