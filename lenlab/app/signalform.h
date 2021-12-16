#ifndef APP_SIGNALFORM_H
#define APP_SIGNALFORM_H

#include <QWidget>

namespace model {
class Lenlab;
class SignalGenerator;
}

namespace app {

namespace Ui {
class SignalForm;
}

class SignalForm : public QWidget
{
    Q_OBJECT

    model::Lenlab* lenlab;
    model::SignalGenerator* signal;

public:
    explicit SignalForm(QWidget *parent = nullptr);
    ~SignalForm();

    void setModel(model::Lenlab* lenlab);

private:
    Ui::SignalForm *ui;
};


} // namespace app
#endif // APP_SIGNALFORM_H
