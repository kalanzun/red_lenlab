#include "mainwindow.h"
#include "app/ui_mainwindow.h"

#include "model/lenlab.h"
#include "labchart.h"
#include "loggerform.h"
#include "oscilloscopeform.h"
#include "spectrumform.h"


namespace app {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    logger_form = new LoggerForm(ui->logger_page->contents());
    oscilloscope_form = new OscilloscopeForm(ui->oscilloscope_page->contents());
    spectrum_form = new SpectrumForm(ui->spectrum_page->contents());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setModel(model::Lenlab* lenlab)
{
    this->lenlab = lenlab;

    logger_form->setModel(lenlab);
    logger_form->setupChart(ui->logger_page->lab_chart());

    oscilloscope_form->setModel(lenlab);
    oscilloscope_form->setupChart(ui->oscilloscope_page->lab_chart());

    spectrum_form->setModel(lenlab);
    spectrum_form->setupChart(ui->spectrum_page->lab_chart());
}

} // namespace app
