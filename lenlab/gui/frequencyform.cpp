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

#include "frequencyform.h"
#include "ui_frequencyform.h"
#include <QDebug>

namespace gui {

FrequencyForm::FrequencyForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrequencyForm)
{
    qDebug() << "FrequencyForm";
    ui->setupUi(this);
}

FrequencyForm::~FrequencyForm()
{
    qDebug() << "~FrequencyForm";
    delete ui;
}

void
FrequencyForm::setMainWindow(MainWindow *main_window)
{
    this->main_window = main_window;
}

void
FrequencyForm::setModel(model::Lenlab *lenlab)
{
    this->lenlab = lenlab;
    this->frequencysweep = lenlab->frequencysweep;
}

void
FrequencyForm::on_startButton_clicked()
{
    if (!frequencysweep->active()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(frequencysweep)) return;
        }
        frequencysweep->start();
    }
}

void
FrequencyForm::on_stopButton_clicked()
{
    if (frequencysweep->active()) {
        frequencysweep->stop();
    }
}

} // namespace gui
