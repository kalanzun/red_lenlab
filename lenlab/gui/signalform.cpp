/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "signalform.h"
#include "ui_signalform.h"
//#include <QDebug>

namespace gui {

SignalForm::SignalForm(QWidget * parent)
    : QWidget(parent)
    , ui(new Ui::SignalForm)
{
    ui->setupUi(this);

    //setUIConfiguration(false, false, false);
}

SignalForm::~SignalForm()
{
    delete ui;
}

void
SignalForm::setMainWindow(MainWindow * main_window)
{
    m_main_window = main_window;
}

void
SignalForm::setModel(model::Lenlab * lenlab)
{
    m_lenlab = lenlab;
    m_signalgenerator = &lenlab->signalgenerator;

    connect(m_signalgenerator, &model::Signalgenerator::activeChanged,
            this, &SignalForm::activeChanged);
    connect(m_signalgenerator, &model::Signalgenerator::lockedChanged,
            this, &SignalForm::lockedChanged);

    ui->signalTypeBox->insertItem(0, "Aus");
    ui->signalTypeBox->insertItem(1, "Sinus");

    ui->amplitudeBox->insertItems(0, m_signalgenerator->getAmplitudeIndex().labels);
    ui->amplitudeSlider->setMaximum(static_cast<int>(m_signalgenerator->getAmplitudeIndex().length - 1));
    ui->amplitudeBox->setCurrentIndex(14);
    ui->amplitudeSlider->setValue(14);

    ui->frequencyBox->insertItems(0, m_signalgenerator->getFrequencyIndex().labels);
    ui->frequencySlider->setMaximum(static_cast<int>(m_signalgenerator->getFrequencyIndex().length - 1));
    ui->frequencyBox->setCurrentIndex(82);
    ui->frequencySlider->setValue(82);

    ui->secondBox->insertItems(0, m_signalgenerator->getSecondIndex().labels);
    ui->secondSlider->setMaximum(static_cast<int>(m_signalgenerator->getSecondIndex().length - 1));
}

/*
void
SignalForm::setUIConfiguration(bool amplitude, bool frequency, bool second)
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

    if (second) {
        ui->secondLabel->show();
        ui->secondBox->show();
        ui->secondSlider->show();
    }
    else {
        ui->secondLabel->hide();
        ui->secondBox->hide();
        ui->secondSlider->hide();
    }
}
*/

void
SignalForm::on_signalTypeBox_activated(int index)
{
    if (index == 1) { // Sinus
        if (m_signalgenerator->locked()) {
            ui->signalTypeBox->setCurrentIndex(0);
            m_active = 0;
        }
        else if (!m_active){
            m_signalgenerator->setAmplitude(static_cast<uint32_t>(ui->amplitudeBox->currentIndex()));
            m_signalgenerator->setFrequency(static_cast<uint32_t>(ui->frequencyBox->currentIndex()));
            m_signalgenerator->setSecond(static_cast<uint32_t>(ui->secondBox->currentIndex()));
            m_active = 1;
            m_signalgenerator->start();
            //setUIConfiguration(true, true, true);
        }
    }
    else {
        if (m_signalgenerator->locked()) {
        }
        else if (m_active) {
            m_active = 0;
            m_signalgenerator->stop();
            //setUIConfiguration(false, false, false);
        }
    }
}

void
SignalForm::activeChanged(bool active)
{
    if (!active && m_active) {
        // reset
        ui->signalTypeBox->setCurrentIndex(0); // does not trigger activated
        m_active = 0;
    }
}

void
SignalForm::lockedChanged(bool locked)
{
    if (locked) {
        ui->signalTypeBox->setCurrentIndex(0); // does not trigger activated
        m_active = 0;
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
    if (m_active) {
        m_signalgenerator->setAmplitude(static_cast<uint32_t>(index));
        m_signalgenerator->setSine();
    }
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
    if (m_active) {
        m_signalgenerator->setFrequency(static_cast<uint32_t>(index));
        m_signalgenerator->setSine();
    }
}

void
SignalForm::on_secondBox_activated(int index)
{
    // this will call valueChanged, which updates the model
    ui->secondSlider->setValue(index);
}

void
SignalForm::on_secondSlider_valueChanged(int index)
{
    ui->secondBox->setCurrentIndex(index);
    if (m_active) {
        m_signalgenerator->setSecond(static_cast<uint32_t>(index));
        m_signalgenerator->setSine();
    }
}


} // namespace gui
