#ifndef LOGGERFORM_H
#define LOGGERFORM_H

#include <QWidget>

namespace model {

class Lenlab;

}

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

private:
    Ui::LoggerForm *ui;
};

} // namespace app
#endif // LOGGERFORM_H
