#include "frequencyform.h"
#include "app/ui_frequencyform.h"

namespace app {

FrequencyForm::FrequencyForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrequencyForm)
{
    ui->setupUi(this);
}

FrequencyForm::~FrequencyForm()
{
    delete ui;
}

void FrequencyForm::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;
}

void FrequencyForm::setupChart(LabChart* lab_chart)
{

}

} // namespace app
