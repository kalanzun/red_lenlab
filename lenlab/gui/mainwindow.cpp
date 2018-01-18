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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

namespace gui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow";
    ui->setupUi(this);

    setWindowTitle("Lenlab");

    ui->signal->hide();
    ui->logPlainTextEdit->hide();

#ifdef QT_NO_DEBUG
    // Hide logger because it is not implemented, yet
#endif
    ui->tabWidget->setTabEnabled(0, false); // logger
}

MainWindow::~MainWindow()
{
    qDebug("~MainWindow");
    delete ui;
}

/**
 * @brief MainWindow::setModel
 * @param lenlab
 *
 * It does not take ownership.
 */

void
MainWindow::setModel(model::Lenlab *lenlab)
{
    this->lenlab = lenlab;

    ui->loggerTab->setMainWindow(this);
    ui->loggerTab->setModel(lenlab);
    ui->oscilloscopeTab->setMainWindow(this);
    ui->oscilloscopeTab->setModel(lenlab);
    ui->FrequencyTab->setMainWindow(this);
    ui->FrequencyTab->setModel(lenlab);
    ui->signal->setMainWindow(this);
    ui->signal->setModel(lenlab);

    connect(lenlab, SIGNAL(logMessage(QString)),
            this, SLOT(on_logMessage(QString)));
}

void
MainWindow::setHandler(usb::Handler *handler)
{
    this->handler = handler;

    connect(handler, SIGNAL(logMessage(QString)),
            this, SLOT(on_logMessage(QString)));
}

bool
MainWindow::askToCancelActiveComponent(model::Component *next_component)
{
    QString previous = lenlab->getActiveComponent()->getNameNominative();
    QString next = next_component->getNameAccusative();
    QMessageBox msgBox;
    msgBox.setText(QString(
        "D%1 ist noch aktiv. Um d%2 zu starten muss d%1 gestoppt werden.").arg(
            previous,
            next));
    msgBox.setInformativeText(QString(
        "Möchten Sie d%1 stoppen und d%2 starten?").arg(
            previous,
            next));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    if (msgBox.exec() == QMessageBox::Ok) {
        lenlab->getActiveComponent()->stop();
        return true;
    }
    else {
        return false;
    }
}

void
MainWindow::on_replot()
{
    //ui->plot->replot();
}

void
MainWindow::on_signalButton_toggled(bool checked)
{
    signal_checked = checked;
    if (checked)
        ui->signal->show();
    else
        ui->signal->hide();
}

void
MainWindow::on_logButton_toggled(bool checked)
{
    if (checked)
        ui->logPlainTextEdit->show();
    else
        ui->logPlainTextEdit->hide();

}

void
MainWindow::on_tabWidget_currentChanged(int index)
{
    // changing the ToolBox only changes the GUI.
    // I cannot prevent the change of the ToolBox here, so I cannot implement a
    // diaglog to ask whether to cancel another measurement.

    if (index == 2) {
        bool _signal_checked = ui->signalButton->isChecked();
        ui->signalButton->setChecked(false);
        signal_checked = _signal_checked;
        ui->signalButton->setEnabled(false);
    }
    else {
        ui->signalButton->setChecked(signal_checked);
        ui->signalButton->setEnabled(true);
    }    
}

void
MainWindow::on_logMessage(const QString &msg)
{
    ui->logLineEdit->setText(msg);
    ui->logPlainTextEdit->appendPlainText(msg);
}

void
MainWindow::on_actionSaveImage_triggered()
{
    int index = ui->tabWidget->currentIndex();

    if (index == 0)
        ui->loggerTab->saveImage();
}

void
MainWindow::on_actionSaveData_triggered()
{
    int index = ui->tabWidget->currentIndex();

    if (index == 0)
        ui->loggerTab->save();
}

void
MainWindow::on_actionQuit_triggered()
{
    close();
}

const QString ABOUT(
        "<h1>Lenlab " STR(MAJOR) "." STR(MINOR) "</h1>\n"

        "<p>Version: " STR(MAJOR) "." STR(MINOR) "." STR(REVISION) ", "

#ifdef QT_NO_DEBUG
        "release build"
#else
        "debug build"
#endif

        "</p>\n"

        R"_(
        <p>Copyright 2017<br/>
        Christoph Simon und das Lenlab-Entwicklerteam</p>

        <p>Homepage: <a href=\"https://git.scc.kit.edu/vq6936/red_lenlab\">https://git.scc.kit.edu/vq6936/red_lenlab</a></p>

        <p>This program is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.</p>

        <p>This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.</p>

        <p>You should have received a copy of the GNU General Public License
        along with this program.  If not, see
        <a href=\"https://www.gnu.org/licenses/\">https://www.gnu.org/licenses/</a>.</p>
        )_");


void
MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Über Lenlab", ABOUT);
}

} // namespace gui
