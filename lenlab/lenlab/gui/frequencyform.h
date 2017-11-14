#ifndef GUI_FREQUENCYFORM_H
#define GUI_FREQUENCYFORM_H

#include "mainwindow.h"
#include "model/lenlab.h"
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

    void setMainWindow(MainWindow *_main_window);
    void setLenlab(model::Lenlab *_lenlab);

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::FrequencyForm *ui;
    MainWindow *main_window;
    model::Lenlab *lenlab;
};


} // namespace gui
#endif // GUI_FREQUENCYFORM_H
