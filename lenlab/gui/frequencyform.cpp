#include "frequencyform.h"
#include "ui_frequencyform.h"
#include <QDebug>

namespace gui {

FrequencyForm::FrequencyForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrequencyForm)
{
    qDebug() << "FrequencyForm";
    ui->setupUi(this);
}

FrequencyForm::~FrequencyForm()
{
    qDebug() << "~FrequencyForm";
    delete ui;
}

void
FrequencyForm::setMainWindow(MainWindow *main_window)
{
    this->main_window = main_window;
}

void
FrequencyForm::setModel(model::Lenlab *lenlab)
{
    this->lenlab = lenlab;
    this->frequency = lenlab->frequency;
}

void
FrequencyForm::on_startButton_clicked()
{
    if (!frequency->active()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(frequency)) return;
        }
        frequency->start();
    }
}

void
FrequencyForm::on_stopButton_clicked()
{
    if (frequency->active()) {
        frequency->stop();
    }
}

} // namespace gui
