#ifndef LOGGERFORM_H
#define LOGGERFORM_H

#include <QWidget>

namespace model {

class Lenlab;
class Logger;

}

namespace app {

namespace Ui {
class LoggerForm;
}

class LoggerForm : public QWidget
{
    Q_OBJECT

    model::Lenlab* lenlab;
    model::Logger* logger;

public:
    explicit LoggerForm(QWidget *parent = nullptr);
    ~LoggerForm();

    void setModel(model::Lenlab* lenlab);

    void setupChart() const;

private:
    Ui::LoggerForm *ui;

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
};

} // namespace app
#endif // LOGGERFORM_H
