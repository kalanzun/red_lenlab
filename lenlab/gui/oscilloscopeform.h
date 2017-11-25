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

#ifndef GUI_OSCILLOSCOPEFORM_H
#define GUI_OSCILLOSCOPEFORM_H

#include "mainwindow.h"
#include "model/lenlab.h"
#include <QWidget>

namespace gui {

namespace Ui {
class OscilloscopeForm;
}

/**
 * @brief Controls for the Lenlab oscilloscope component.
 */

class OscilloscopeForm : public QWidget
{
    Q_OBJECT

public:
    explicit OscilloscopeForm(QWidget *parent = 0);
    ~OscilloscopeForm();

    void setMainWindow(MainWindow *main_window);
    void setModel(model::Lenlab *lenlab);

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::OscilloscopeForm *ui;

    MainWindow *main_window;
    model::Lenlab *lenlab;
    model::Oscilloscope *oscilloscope;
};


} // namespace gui
#endif // GUI_OSCILLOSCOPEFORM_H
