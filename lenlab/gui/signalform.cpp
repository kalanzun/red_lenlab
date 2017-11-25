/*

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "signalform.h"
#include "ui_signalform.h"
#include <QDebug>

namespace gui {

SignalForm::SignalForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignalForm)
{
    qDebug() << "SignalForm";
    ui->setupUi(this);

    setUIConfiguration(true, false, false);
}

SignalForm::~SignalForm()
{
    qDebug() << "~SignalForm";
    delete ui;
}

void
SignalForm::setMainWindow(MainWindow *main_window)
{
    this->main_window = main_window;
}

void
SignalForm::setModel(model::Lenlab *lenlab)
{
    this->lenlab = lenlab;
    this->signal = lenlab->signal;
}

void
SignalForm::setTitle(QString title)
{
    ui->title->setText(title);
}

void
SignalForm::setUIConfiguration(bool amplitude, bool frequency, bool dutycycle)
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
