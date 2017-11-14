#ifndef GUI_LOGGERFORM_H
#define GUI_LOGGERFORM_H

#include <QWidget>

namespace gui {

namespace Ui {
class LoggerForm;
}

class LoggerForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoggerForm(QWidget *parent = 0);
    ~LoggerForm();

private:
    Ui::LoggerForm *ui;
};


} // namespace gui
#endif // GUI_LOGGERFORM_H
