#include "mainwindow.h"
#include "app/ui_mainwindow.h"

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
    ui->logger_page->lab_chart()->setModel(lenlab->logger);

    oscilloscope_form->setModel(lenlab);
    ui->oscilloscope_page->lab_chart()->setModel(lenlab->oscilloscope);

    spectrum_form->setModel(lenlab);
    ui->spectrum_page->lab_chart()->setModel(lenlab->spectrum);
}

} // namespace app
