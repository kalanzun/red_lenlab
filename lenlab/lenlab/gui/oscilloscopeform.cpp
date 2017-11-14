#include "oscilloscopeform.h"
#include "ui_oscilloscopeform.h"

namespace gui {

OscilloscopeForm::OscilloscopeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OscilloscopeForm)
{
    ui->setupUi(this);
}

OscilloscopeForm::~OscilloscopeForm()
{
    delete ui;
}

void
OscilloscopeForm::setMainWindow(MainWindow *_main_window)
{
    main_window = _main_window;
}

void
OscilloscopeForm::setLenlab(model::Lenlab *_lenlab)
{
    lenlab = _lenlab;
}

void
OscilloscopeForm::on_startButton_clicked()
{
    if (!lenlab->oscilloscope->isActive()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(lenlab->oscilloscope)) return;
        }
        lenlab->oscilloscope->start();
    }
}

void
OscilloscopeForm::on_stopButton_clicked()
{
    lenlab->oscilloscope->stop();
}

} // namespace gui
