#include "oscilloscopeform.h"
#include "app/ui_oscilloscopeform.h"

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

void OscilloscopeForm::setupChart(LabChart* lab_chart)
{

}

} // namespace app
