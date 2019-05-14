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
#include "model/series.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_grid.h"
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

    Ui::OscilloscopeForm * ui;

    bool pending = false;

    MainWindow * m_main_window = nullptr;
    model::Lenlab * m_lenlab = nullptr;
    model::Oscilloscope * m_oscilloscope = nullptr;

    std::array<QwtPlotCurve *, 2> m_curves; // pointer, no ownership

    QwtPlotCurve *newCurve(const QColor &color, bool visible);
    QwtPlotGrid *newGrid();

public:
    explicit OscilloscopeForm(QWidget  *parent = nullptr);
    ~OscilloscopeForm();

    void setMainWindow(MainWindow * main_window);
    void setModel(model::Lenlab * lenlab);

    void save();
    void saveImage();

private slots:
    void seriesChanged(model::pSeries const &);

    void on_startButton_clicked();
    void on_stopButton_clicked();

    void on_samplerateBox_activated(int index);

    void on_ch1CheckBox_stateChanged(int state);
    void on_ch2CheckBox_stateChanged(int state);

    void on_saveButton_clicked();

    void on_timerangeBox_currentIndexChanged(int index);

    void activeChanged(bool);
};


} // namespace gui
#endif // GUI_OSCILLOSCOPEFORM_H
