#include "mainwindow.h"
#include "app/ui_mainwindow.h"

#include "protocol/board.h"
#include "model/lenlab.h"

namespace app {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->signalFrame->hide();

    deviceLabel = new QLabel("getrennt");
    ui->statusbar->addPermanentWidget(deviceLabel);
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
    ui->signalForm->setModel(lenlab);
    ui->spectrumForm->setModel(lenlab);

    connect(this->lenlab->board, &protocol::Board::setup,
            this, &MainWindow::handleSetup);
    connect(this->lenlab->board, &protocol::Board::error,
            this, &MainWindow::handleError);
}

void MainWindow::on_signalButton_toggled(bool checked)
{
    if (checked) ui->signalFrame->show();
    else ui->signalFrame->hide();
}

void MainWindow::handleSetup(const std::shared_ptr< protocol::Message >& message)
{
    deviceLabel->setText("verbunden");
}

void MainWindow::handleError()
{
    deviceLabel->setText("getrennt");
}

} // namespace app
