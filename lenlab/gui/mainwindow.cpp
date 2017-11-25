#include "mainwindow.h"
#include "ui_mainwindow.h"
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

    ui->signalA->hide();
    ui->signalA->setTitle("Signalgenerator A");
    ui->signalB->hide();
    ui->signalB->setTitle("Signalgenerator B");

#ifdef QT_NO_DEBUG
    // Hide signal generator, oscilloscope and frequency analysis because they are not implemented, yet
    ui->tabWidget->setTabEnabled(1, false); // oscilloscope
    ui->tabWidget->setTabEnabled(2, false); // frequency analysis
    ui->signalAButton->hide();
    ui->signalBButton->hide();
#endif
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
    ui->signalA->setMainWindow(this);
    ui->signalA->setModel(lenlab);
    ui->signalB->setMainWindow(this);
    ui->signalB->setModel(lenlab);

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
MainWindow::on_signalAButton_toggled(bool checked)
{
    signalA_checked = checked;
    if (checked)
        ui->signalA->show();
    else
        ui->signalA->hide();
}

void
MainWindow::on_signalBButton_toggled(bool checked)
{
    signalB_checked = checked;
    if (checked)
        ui->signalB->show();
    else
        ui->signalB->hide();
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
        bool _signalA_checked = ui->signalAButton->isChecked();
        bool _signalB_checked = ui->signalBButton->isChecked();
        ui->signalAButton->setChecked(false);
        ui->signalBButton->setChecked(false);
        signalA_checked = _signalA_checked;
        signalB_checked = _signalB_checked;
        ui->signalAButton->setEnabled(false);
        ui->signalBButton->setEnabled(false);
    }
    else {
        ui->signalAButton->setChecked(signalA_checked);
        ui->signalBButton->setChecked(signalB_checked);
        ui->signalAButton->setEnabled(true);
        ui->signalBButton->setEnabled(true);
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

void
MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Über Lenlab",
        "<h1>Lenlab 7.0</h1>"
        "<p>Copyright 2017<br/>Karlsruher Institut für Technologie<br/>"
        "Lichttechnisches Institut<br/>Christoph Simon<br/>"
        "und das Lenlab-Entwicklerteam</p>"
        "<p>This program is free software: you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation, either version 3 of the License, or "
        "(at your option) any later version.</p>"
        "<p>This program is distributed in the hope that it will be useful, "
        "but WITHOUT ANY WARRANTY; without even the implied warranty of "
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
        "GNU General Public License for more details.</p>"
        "<p>You should have received a copy of the GNU General Public License "
        "along with this program.  If not, see "
        "<a href=\"https://www.gnu.org/licenses/\">https://www.gnu.org/licenses/</a>.</p>"
        "<p><a href=\"https://git.scc.kit.edu/vq6936/red_lenlab\">https://git.scc.kit.edu/vq6936/red_lenlab</a></p>");
}

} // namespace gui
