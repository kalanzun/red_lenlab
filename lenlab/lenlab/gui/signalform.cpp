#include "signalform.h"
#include "ui_signalform.h"

namespace gui {

SignalForm::SignalForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignalForm)
{
    ui->setupUi(this);

    setParameterUIConfiguration(true, false, false);
}

SignalForm::~SignalForm()
{
    delete ui;
}

void
SignalForm::setTitle(QString title)
{
    ui->title->setText(title);
}

void
SignalForm::setParameterUIConfiguration(bool amplitude, bool frequency, bool dutycycle)
{
    if (amplitude) {
        ui->amplitudeButton->show();
        ui->amplitudeEdit->show();
        ui->amplitudeSlider->show();
    }
    else {
        ui->amplitudeButton->hide();
        ui->amplitudeEdit->hide();
        ui->amplitudeSlider->hide();
    }

    if (frequency) {
        ui->frequencyButton->show();
        ui->frequencyEdit->show();
        ui->frequencySlider->show();
    }
    else {
        ui->frequencyButton->hide();
        ui->frequencyEdit->hide();
        ui->frequencySlider->hide();
    }

    if (dutycycle) {
        ui->dutycycleButton->show();
        ui->dutycycleEdit->show();
        ui->dutycycleSlider->show();
    }
    else {
        ui->dutycycleButton->hide();
        ui->dutycycleEdit->hide();
        ui->dutycycleSlider->hide();
    }
}

} // namespace gui
