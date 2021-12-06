#include "mainwindow.h"
#include "app/ui_mainwindow.h"

namespace app {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    logger_form = new LoggerForm(ui->logger_tab->contents());
    oscilloscope_form = new OscilloscopeForm(ui->oscilloscope_tab->contents());
    frequency_form = new FrequencyForm(ui->frequency_tab->contents());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;

    logger_form->setModel(lenlab);
    logger_form->setupChart(ui->logger_tab->lab_chart());

    oscilloscope_form->setModel(lenlab);
    oscilloscope_form->setupChart(ui->oscilloscope_tab->lab_chart());

    frequency_form->setModel(lenlab);
    frequency_form->setupChart(ui->frequency_tab->lab_chart());
}

} // namespace app
