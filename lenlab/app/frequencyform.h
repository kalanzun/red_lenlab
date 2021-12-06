#ifndef FREQUENCYFORM_H
#define FREQUENCYFORM_H

#include <QWidget>

#include "model/lenlab.h"
#include "labchart.h"

namespace app {

namespace Ui {
class FrequencyForm;
}

class FrequencyForm : public QWidget
{
    Q_OBJECT

    model::Lenlab* lenlab;

public:
    explicit FrequencyForm(QWidget *parent = nullptr);
    ~FrequencyForm();

    void setModel(model::Lenlab* lenlab);
    void setupChart(LabChart* lab_chart);

private:
    Ui::FrequencyForm *ui;
};

} // namespace app
#endif // FREQUENCYFORM_H
