#include "frequencyform.h"
#include "ui_frequencyform.h"

namespace gui {

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

} // namespace gui
