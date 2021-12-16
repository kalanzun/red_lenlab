#ifndef APP_SIGNALFORM_H
#define APP_SIGNALFORM_H

#include <QWidget>

namespace app {

namespace Ui {
class SignalForm;
}

class SignalForm : public QWidget
{
    Q_OBJECT

public:
    explicit SignalForm(QWidget *parent = nullptr);
    ~SignalForm();

private:
    Ui::SignalForm *ui;
};


} // namespace app
#endif // APP_SIGNALFORM_H
