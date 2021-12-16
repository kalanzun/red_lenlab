#include "signalform.h"
#include "ui_signalform.h"

#include "model/lenlab.h"
#include "model/signalgenerator.h"

namespace app {

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

void SignalForm::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;
    this->signal = lenlab->signal;

    ui->signalBox->addItem("Aus");
    ui->signalBox->addItem("Konstant");
    ui->signalBox->addItem("Rechteck");
    ui->signalBox->addItem("Dreieck");
    ui->signalBox->addItem("Sinus");

    for (auto&& item : signal->amplitude) ui->amplitudeBox->addItem(item);
    for (auto&& item : signal->frequency) ui->frequencyBox->addItem(item);
    for (auto&& item : signal->multiplier) ui->bBox->addItem(item);
}

} // namespace app
