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
}

} // namespace app
