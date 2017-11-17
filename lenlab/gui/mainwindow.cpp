#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

namespace gui {

MainWindow::MainWindow(model::Lenlab *lenlab, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    lenlab(lenlab)
{
    ui->setupUi(this);

    ui->frequencyPage->setMainWindow(this);
    ui->frequencyPage->setLenlab(lenlab);
    ui->loggerPage->setMainWindow(this);
    ui->loggerPage->setLenlab(lenlab);
    ui->oscilloscopePage->setMainWindow(this);
    ui->oscilloscopePage->setLenlab(lenlab);
    ui->signalA->setMainWindow(this);
    ui->signalA->setLenlab(lenlab);
    ui->signalB->setMainWindow(this);
    ui->signalB->setLenlab(lenlab);

    ui->loggerPage->configurePlot(ui->plot);

    ui->signalA->hide();
    ui->signalA->setTitle("Signalgenerator A");
    ui->signalB->hide();
    ui->signalB->setTitle("Signalgenerator B");

    connect(lenlab, SIGNAL(replot()),
            this, SLOT(on_replot()));

    connect(lenlab, SIGNAL(logMessage(QString)),
            this, SLOT(on_logMessage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
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
    ui->plot->replot();
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
MainWindow::on_toolBox_currentChanged(int index)
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
    ui->logPlainTextEdit->appendPlainText(msg);
}

} // namespace gui
