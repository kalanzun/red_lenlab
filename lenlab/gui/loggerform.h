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

#ifndef GUI_LOGGERFORM_H
#define GUI_LOGGERFORM_H

#include "mainwindow.h"
#include "model/lenlab.h"
#include "model/series.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_grid.h"
#include <QWidget>

namespace gui {

namespace Ui {
class LoggerForm;
}

class LoggerForm : public QWidget
{
    Q_OBJECT

    Ui::LoggerForm * ui;

    MainWindow * m_main_window = nullptr;
    model::Lenlab * m_lenlab = nullptr;
    model::Voltmeter * m_voltmeter = nullptr;

    std::array<QwtPlotCurve *, 2> m_curves; // pointer, no ownership

public:
    explicit LoggerForm(QWidget * parent = nullptr);
    ~LoggerForm();

    void setMainWindow(MainWindow *main_window);
    void setModel(model::Lenlab * lenlab);

    void saveImage();
    void save();

private:
    QwtPlotGrid * newGrid();
    QwtPlotCurve * newCurve(QColor const & color, bool visible);

private slots:
    void on_intervalComboBox_activated(int);
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_ch1CheckBox_stateChanged(int);
    void on_ch2CheckBox_stateChanged(int);
    void on_ch3CheckBox_stateChanged(int);
    void on_ch4CheckBox_stateChanged(int);
    void on_saveButton_clicked();
    void on_autoSaveCheckBox_stateChanged(int);
    void on_clearButton_clicked();

    void seriesUpdated();
    void seriesChanged(model::pSeries const &);

    void on_measurementDataChanged(bool);
    void on_unsavedDataChanged(bool);
    void on_autoSaveChanged(bool);
    void on_fileNameChanged(const QString &);
    void on_channelsChanged(const std::bitset<4> &);
};


} // namespace gui
#endif // GUI_LOGGERFORM_H
