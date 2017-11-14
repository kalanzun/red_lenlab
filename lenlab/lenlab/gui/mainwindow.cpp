#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace gui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->signalA->hide();
    ui->signalA->setTitle("Signalgenerator A");
    ui->signalB->hide();
    ui->signalB->setTitle("Signalgenerator B");
}

MainWindow::~MainWindow()
{
    delete ui;
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

} // namespace gui
