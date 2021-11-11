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

#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include "model/lenlab.h"

#include <QMainWindow>

namespace gui {

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow * ui;
    model::Lenlab * m_lenlab = nullptr;

    bool m_signal_checked = false;

public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow();

    void setModel(model::Lenlab * lenlab);

    bool askToCancelActiveComponent(model::Component *next_component);

private slots:
    void on_replot();
    void on_signalButton_toggled(bool);
    void on_logButton_toggled(bool);
    void on_tabWidget_currentChanged(int);
    void on_logMessage(QString const &);
    void on_actionSaveData_triggered();
    void on_actionSaveImage_triggered();
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
};

} // namespace gui
#endif // GUI_MAINWINDOW_H
