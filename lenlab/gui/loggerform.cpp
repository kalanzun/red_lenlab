#include "loggerform.h"
#include "ui_loggerform.h"
#include "mainwindow.h"
#include <QMessageBox>

namespace gui {

LoggerForm::LoggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerForm)
{
    ui->setupUi(this);
}

LoggerForm::~LoggerForm()
{
    delete ui;
}

void
LoggerForm::setMainWindow(MainWindow *_main_window)
{
    main_window = _main_window;
}

void
LoggerForm::setLenlab(model::Lenlab *_lenlab)
{
    lenlab = _lenlab;
}

void
LoggerForm::on_startButton_clicked()
{
    if (!lenlab->logger->isActive()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(lenlab->logger)) return;
        }
        lenlab->logger->start();
    }
}

void
LoggerForm::on_stopButton_clicked()
{
    if (lenlab->logger->isActive()) {
        lenlab->logger->stop();
    }
}

} // namespace gui
