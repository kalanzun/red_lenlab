/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
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

#ifndef GUI_SIGNALFORM_H
#define GUI_SIGNALFORM_H

#include "mainwindow.h"
#include "model/lenlab.h"

#include <QWidget>

namespace gui {

namespace Ui {
class SignalForm;
}

class SignalForm : public QWidget
{
    Q_OBJECT

    //void setUIConfiguration(bool amplitude, bool frequency, bool divider);

    Ui::SignalForm * ui;

    MainWindow * m_main_window = nullptr;
    model::Lenlab * m_lenlab = nullptr;
    model::Signalgenerator * m_signalgenerator = nullptr;

    bool m_active = false;

public:
    explicit SignalForm(QWidget * parent = nullptr);
    ~SignalForm();

    void setMainWindow(MainWindow * main_window);
    void setModel(model::Lenlab * lenlab);

private slots:
    void on_signalTypeBox_activated(int index);
    void on_amplitudeBox_activated(int index);
    void on_amplitudeSlider_valueChanged(int index);
    void on_frequencyBox_activated(int index);
    void on_frequencySlider_valueChanged(int index);
    void on_secondBox_activated(int index);
    void on_secondSlider_valueChanged(int index);

    void activeChanged(bool active);
    void lockedChanged(bool locked);
};

} // namespace gui
#endif // GUI_SIGNALFORM_H
