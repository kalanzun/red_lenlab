#include "loggerform.h"
#include "ui_loggerform.h"

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

} // namespace gui
