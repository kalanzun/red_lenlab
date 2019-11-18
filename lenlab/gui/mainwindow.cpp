/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lenlab_version.h"
#include <QMessageBox>
#include <QFileDialog>
//#include <QDebug>

namespace gui {

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Lenlab");

    ui->signal->hide();
    ui->logPlainTextEdit->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::setModel(model::Lenlab * lenlab)
{
    m_lenlab = lenlab;

    ui->loggerTab->setMainWindow(this);
    ui->loggerTab->setModel(lenlab);
    ui->oscilloscopeTab->setMainWindow(this);
    ui->oscilloscopeTab->setModel(lenlab);
    ui->FrequencyTab->setMainWindow(this);
    ui->FrequencyTab->setModel(lenlab);
    ui->signal->setMainWindow(this);
    ui->signal->setModel(lenlab);

    connect(lenlab, &model::Lenlab::logMessage,
            this, &MainWindow::on_logMessage);
}

bool
MainWindow::askToCancelActiveComponent(model::Component *next_component)
{
    QString previous = m_lenlab->getActiveComponent()->getNameNominative();
    QString next = next_component->getNameAccusative();
    QMessageBox msgBox;
    msgBox.setText(QString(
        "%1%2 ist noch aktiv. Um %3 zu starten muss %4 gestoppt werden.").arg(
            previous.at(0).toUpper(),
            previous.right(previous.size() - 1),
            next,
            previous));
    msgBox.setInformativeText(QString(
        "Möchten Sie %1 stoppen und %2 starten?").arg(
            previous,
            next));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    return (msgBox.exec() == QMessageBox::Ok);
}

void
MainWindow::on_replot()
{
    //ui->plot->replot();
}

void
MainWindow::on_signalButton_toggled(bool checked)
{
    m_signal_checked = checked;
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
        bool signal_checked = ui->signalButton->isChecked();
        ui->signalButton->setChecked(false);
        m_signal_checked = signal_checked;
        ui->signalButton->setEnabled(false);
    }
    else {
        ui->signalButton->setChecked(m_signal_checked);
        ui->signalButton->setEnabled(true);
    }    
}

void
MainWindow::on_logMessage(QString const & msg)
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
    else if (index == 1)
        ui->oscilloscopeTab->saveImage();
    else if (index == 2)
        ui->FrequencyTab->saveImage();
}

void
MainWindow::on_actionSaveData_triggered()
{
    int index = ui->tabWidget->currentIndex();

    if (index == 0)
        ui->loggerTab->save();
    else if (index == 1)
        ui->oscilloscopeTab->save();
    else if (index == 2)
        ui->FrequencyTab->save();
}


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


void
MainWindow::on_actionQuit_triggered()
{
    close();
}

const QString ABOUT(
        "<h1>Lenlab " STR(MAJOR) "." STR(MINOR) "</h1>\n"

        "<p>"
#ifdef QT_NO_DEBUG
        "release build"
#else
        "debug build"
#endif
        "</p>\n"

        R"_(
        <p>Copyright 2019<br/>
        Christoph Simon und das Lenlab-Entwicklerteam</p>

        <p>Homepage: <a href='https://github.com/kalanzun/red_lenlab'>https://github.com/kalanzun/red_lenlab</a></p>

        <p>Dokumentation: <a href='https://red-lenlab.readthedocs.io'>https://red-lenlab.readthedocs.io</a></p>

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
        <a href='https://www.gnu.org/licenses/'>https://www.gnu.org/licenses/</a>.</p>
        )_");


void
MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Über Lenlab", ABOUT);
}

} // namespace gui
