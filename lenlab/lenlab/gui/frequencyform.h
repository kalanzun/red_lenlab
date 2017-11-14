#ifndef GUI_FREQUENCYFORM_H
#define GUI_FREQUENCYFORM_H

#include <QWidget>

namespace gui {

namespace Ui {
class FrequencyForm;
}

class FrequencyForm : public QWidget
{
    Q_OBJECT

public:
    explicit FrequencyForm(QWidget *parent = 0);
    ~FrequencyForm();

private:
    Ui::FrequencyForm *ui;
};


} // namespace gui
#endif // GUI_FREQUENCYFORM_H
