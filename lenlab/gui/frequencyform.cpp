#include "frequencyform.h"
#include "ui_frequencyform.h"

namespace gui {

FrequencyForm::FrequencyForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrequencyForm)
{
    ui->setupUi(this);
}

FrequencyForm::~FrequencyForm()
{
    delete ui;
}

void
FrequencyForm::setMainWindow(MainWindow *_main_window)
{
    main_window = _main_window;
}

void
FrequencyForm::setLenlab(model::Lenlab *_lenlab)
{
    lenlab = _lenlab;
}

void
FrequencyForm::on_startButton_clicked()
{
    if (!lenlab->frequency->isActive()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(lenlab->frequency)) return;
        }
        lenlab->frequency->start();
    }
}

void
FrequencyForm::on_stopButton_clicked()
{
    if (lenlab->frequency->isActive()) {
        lenlab->frequency->stop();
    }
}

} // namespace gui
