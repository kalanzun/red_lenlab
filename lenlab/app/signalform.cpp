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
    connect(ui->amplitudeBox, &QComboBox::activated,
            &signal->amplitude, &model::Parameter::setIndex);
    ui->amplitudeBox->setCurrentIndex(signal->amplitude.getIndex());

    for (auto&& item : signal->frequency) ui->frequencyBox->addItem(item);
    connect(ui->frequencyBox, &QComboBox::activated,
            &signal->frequency, &model::Parameter::setIndex);
    ui->frequencyBox->setCurrentIndex(signal->frequency.getIndex());

    for (auto&& item : signal->multiplier) ui->bBox->addItem(item);
    connect(ui->bBox, &QComboBox::activated,
            &signal->multiplier, &model::Parameter::setIndex);
    ui->bBox->setCurrentIndex(signal->multiplier.getIndex());
}

} // namespace app
