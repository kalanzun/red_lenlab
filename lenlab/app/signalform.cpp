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
    ui->amplitudeBox->setCurrentIndex(signal->amplitude.getIndex());
    ui->amplitudeSlider->setMaximum(signal->amplitude.getLength() - 1);
    ui->amplitudeSlider->setValue(signal->amplitude.getIndex());
    connect(ui->amplitudeBox, &QComboBox::activated,
            ui->amplitudeSlider, &QSlider::setValue); // triggers valueChanged
    connect(ui->amplitudeSlider, &QSlider::valueChanged,
            &signal->amplitude, &model::Parameter::setIndex);
    connect(ui->amplitudeSlider, &QSlider::valueChanged,
            ui->amplitudeBox, &QComboBox::setCurrentIndex);

    for (auto&& item : signal->frequency) ui->frequencyBox->addItem(item);
    ui->frequencyBox->setCurrentIndex(signal->frequency.getIndex());
    ui->frequencySlider->setMaximum(signal->frequency.getLength() - 1);
    ui->frequencySlider->setValue(signal->frequency.getIndex());
    connect(ui->frequencyBox, &QComboBox::activated,
            ui->frequencySlider, &QSlider::setValue); // triggers valueChanged
    connect(ui->frequencySlider, &QSlider::valueChanged,
            &signal->frequency, &model::Parameter::setIndex);
    connect(ui->frequencySlider, &QSlider::valueChanged,
            ui->frequencyBox, &QComboBox::setCurrentIndex);

    for (auto&& item : signal->multiplier) ui->bBox->addItem(item);
    ui->bBox->setCurrentIndex(signal->multiplier.getIndex());
    ui->bSlider->setMaximum(signal->multiplier.getLength() - 1);
    ui->bSlider->setValue(signal->multiplier.getIndex());
    connect(ui->bBox, &QComboBox::activated,
            ui->bSlider, &QSlider::setValue); // triggers valueChanged
    connect(ui->bSlider, &QSlider::valueChanged,
            &signal->multiplier, &model::Parameter::setIndex);
    connect(ui->bSlider, &QSlider::valueChanged,
            ui->bBox, &QComboBox::setCurrentIndex);
}

} // namespace app
