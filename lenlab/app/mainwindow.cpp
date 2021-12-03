#include "mainwindow.h"
#include "app/ui_mainwindow.h"

namespace app {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    lab_chart = new LabChart(this);
    lab_chart->setChartView(ui->graphicsView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;

    connect(lenlab->logger, &model::Logger::newWaveform,
            lab_chart, &LabChart::setWaveform);
}


} // namespace app
