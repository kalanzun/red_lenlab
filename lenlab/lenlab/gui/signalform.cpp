#include "signalform.h"
#include "ui_signalform.h"

namespace gui {

SignalForm::SignalForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignalForm)
{
    ui->setupUi(this);
}

SignalForm::~SignalForm()
{
    delete ui;
}

} // namespace gui
