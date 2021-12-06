#include "labpage.h"
#include "app/ui_labpage.h"

namespace app {

LabPage::LabPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LabPage)
{
    ui->setupUi(this);
}

LabPage::~LabPage()
{
    delete ui;
}

QWidget* LabPage::contents()
{
    return ui->contents;
}

LabChart* LabPage::lab_chart()
{
    return ui->lab_chart;
}

} // namespace app
