#include "oscilloscopeform.h"
#include "ui_oscilloscopeform.h"
#include <QDebug>

namespace gui {

OscilloscopeForm::OscilloscopeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OscilloscopeForm)
{
    qDebug() << "OscilloscopeForm";
    ui->setupUi(this);
}

OscilloscopeForm::~OscilloscopeForm()
{
    qDebug() << "~OscilloscopeForm";
    delete ui;
}

void
OscilloscopeForm::setMainWindow(MainWindow *main_window)
{
    this->main_window = main_window;
}

void
OscilloscopeForm::setModel(model::Lenlab *lenlab)
{
    this->lenlab = lenlab;
    this->oscilloscope = lenlab->oscilloscope;
}

void
OscilloscopeForm::on_startButton_clicked()
{
    if (!oscilloscope->active()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(oscilloscope)) return;
        }
        oscilloscope->start();
    }
}

void
OscilloscopeForm::on_stopButton_clicked()
{
    if (oscilloscope->active()) {
        oscilloscope->stop();
    }
}

} // namespace gui
