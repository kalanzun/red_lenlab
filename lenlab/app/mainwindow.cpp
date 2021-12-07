#include "mainwindow.h"
#include "app/ui_mainwindow.h"

#include "model/lenlab.h"

namespace app {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

} // namespace app
