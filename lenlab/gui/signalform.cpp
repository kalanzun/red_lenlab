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

    setUIConfiguration(false, false, false);
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
    this->signalgenerator = lenlab->signalgenerator;

    ui->signalTypeBox->insertItem(0, "Aus");
    ui->signalTypeBox->insertItem(1, "Sinus");

    ui->amplitudeBox->insertItems(0, signalgenerator->amplitudeIndex.labels);
    ui->amplitudeSlider->setMaximum(signalgenerator->amplitudeIndex.length - 1);
    ui->amplitudeBox->setCurrentIndex(14);
    ui->amplitudeSlider->setValue(14);

    ui->frequencyBox->insertItems(0, signalgenerator->frequencyIndex.labels);
    ui->frequencySlider->setMaximum(signalgenerator->frequencyIndex.length - 1);
    ui->frequencyBox->setCurrentIndex(25);
    ui->frequencySlider->setValue(25);

    ui->dividerBox->insertItems(0, signalgenerator->dividerIndex.labels);
    ui->dividerSlider->setMaximum(signalgenerator->dividerIndex.length - 1);
}

void
SignalForm::setUIConfiguration(bool amplitude, bool frequency, bool divider)
{
    if (amplitude) {
        ui->amplitudeLabel->show();
        ui->amplitudeBox->show();
        ui->amplitudeSlider->show();
    }
    else {
        ui->amplitudeLabel->hide();
        ui->amplitudeBox->hide();
        ui->amplitudeSlider->hide();
    }

    if (frequency) {
        ui->frequencyLabel->show();
        ui->frequencyBox->show();
        ui->frequencySlider->show();
    }
    else {
        ui->frequencyLabel->hide();
        ui->frequencyBox->hide();
        ui->frequencySlider->hide();
    }

    if (divider) {
        ui->dividerLabel->show();
        ui->dividerBox->show();
        ui->dividerSlider->show();
    }
    else {
        ui->dividerLabel->hide();
        ui->dividerBox->hide();
        ui->dividerSlider->hide();
    }
}

void
SignalForm::on_signalTypeBox_activated(int index)
{
    if (index == 1) { // Sinus
        //signalgenerator->start();
        setUIConfiguration(true, true, true);
    }
    else {
        //signalgenerator->stop();
        setUIConfiguration(false, false, false);
    }
}

void
SignalForm::on_amplitudeBox_activated(int index)
{
    // this will call valueChanged, which updates the model
    ui->amplitudeSlider->setValue(index);
}

void
SignalForm::on_amplitudeSlider_valueChanged(int index)
{
    ui->amplitudeBox->setCurrentIndex(index);
    signalgenerator->setAmplitude(index);
}

void
SignalForm::on_frequencyBox_activated(int index)
{
    // this will call valueChanged, which updates the model
    ui->frequencySlider->setValue(index);
}

void
SignalForm::on_frequencySlider_valueChanged(int index)
{
    ui->frequencyBox->setCurrentIndex(index);
    signalgenerator->setFrequency(index);
}

void
SignalForm::on_dividerBox_activated(int index)
{
    // this will call valueChanged, which updates the model
    ui->dividerSlider->setValue(index);
}

void
SignalForm::on_dividerSlider_valueChanged(int index)
{
    ui->dividerBox->setCurrentIndex(index);
    signalgenerator->setDivider(index);
}


} // namespace gui
