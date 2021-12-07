#include "spectrumform.h"
#include "ui_spectrumform.h"

#include "model/lenlab.h"

namespace app {

SpectrumForm::SpectrumForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpectrumForm)
{
    ui->setupUi(this);
}

SpectrumForm::~SpectrumForm()
{
    delete ui;
}

void SpectrumForm::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;
}

} // namespace app
