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
    if (checked)
        ui->signalA->show();
    else
        ui->signalA->hide();
}

void
MainWindow::on_signalBButton_toggled(bool checked)
{
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

} // namespace gui
