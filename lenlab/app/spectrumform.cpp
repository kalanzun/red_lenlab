#include "spectrumform.h"
#include "ui_spectrumform.h"

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

void SpectrumForm::setupChart(LabChart* lab_chart)
{

}

} // namespace app
