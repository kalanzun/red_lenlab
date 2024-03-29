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

#ifndef GUI_FREQUENCYFORM_H
#define GUI_FREQUENCYFORM_H

#include "labchart.h"
#include "mainwindow.h"
#include "model/lenlab.h"

#include <QWidget>

namespace gui {

namespace Ui {
class FrequencyForm;
}

class FrequencyForm : public QWidget
{
    Q_OBJECT

    Ui::FrequencyForm * ui;

    bool pending = false;

    MainWindow * m_main_window = nullptr;
    model::Lenlab * m_lenlab = nullptr;
    model::Frequencysweep * m_frequencysweep = nullptr;

public:
    explicit FrequencyForm(QWidget * parent = nullptr);
    ~FrequencyForm();

    void prepareChart(LabChart *chart);

    void setMainWindow(MainWindow * main_window);
    void setModel(model::Lenlab * lenlab);

    void save();
    void saveImage();

    void setTheme(QChart::ChartTheme theme);

private slots:
    void seriesChanged(model::pSeries const &);
    void seriesUpdated(model::pSeries const &);

    void on_startButton_clicked();
    void on_stopButton_clicked();

    void on_saveButton_clicked();

    void activeChanged(bool);
};


} // namespace gui
#endif // GUI_FREQUENCYFORM_H
