#include "mainwindow.h"
#include "app/ui_mainwindow.h"

#include "model/lenlab.h"

namespace app {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->signalFrame->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;

    ui->loggerForm->setModel(lenlab);
    ui->oscilloscopeForm->setModel(lenlab);
    ui->spectrumForm->setModel(lenlab);
}

void MainWindow::on_signalButton_toggled(bool checked)
{
    if (checked) ui->signalFrame->show();
    else ui->signalFrame->hide();
}

} // namespace app
