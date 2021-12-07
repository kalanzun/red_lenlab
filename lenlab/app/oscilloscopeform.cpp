#include "oscilloscopeform.h"
#include "app/ui_oscilloscopeform.h"

#include "model/lenlab.h"

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

    setupChart();
}

void OscilloscopeForm::setupChart() const
{
    ui->labChart->setXLabel("Zeit [ms]");
    ui->labChart->setYLabel("Spannung [V]");

    ui->labChart->createChannel("Kanal 1");
    ui->labChart->createChannel("Kanal 2", false);

    ui->labChart->createDefaultAxes();
}

} // namespace app
