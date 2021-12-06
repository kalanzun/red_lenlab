#ifndef LOGGERFORM_H
#define LOGGERFORM_H

#include <QWidget>

#include "model/lenlab.h"
#include "labchart.h"

namespace app {

namespace Ui {
class LoggerForm;
}

class LoggerForm : public QWidget
{
    Q_OBJECT

    model::Lenlab* lenlab;

public:
    explicit LoggerForm(QWidget *parent = nullptr);
    ~LoggerForm();

    void setModel(model::Lenlab* lenlab);
    void setupChart(LabChart* lab_chart);

private:
    Ui::LoggerForm *ui;
};

} // namespace app
#endif // LOGGERFORM_H
