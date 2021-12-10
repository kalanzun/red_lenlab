#ifndef OSCILLOSCOPEFORM_H
#define OSCILLOSCOPEFORM_H

#include <QWidget>

namespace model {
class Lenlab;
class Oscilloscope;
}

namespace app {

namespace Ui { class OscilloscopeForm; }

class OscilloscopeForm : public QWidget
{
    Q_OBJECT

    model::Lenlab* lenlab;
    model::Oscilloscope* oscilloscope;

public:
    explicit OscilloscopeForm(QWidget *parent = nullptr);
    ~OscilloscopeForm();

    void setModel(model::Lenlab* lenlab);

    void setupChart() const;

private:
    Ui::OscilloscopeForm *ui;

private slots:
    void on_startButton_clicked();
};

} // namespace app
#endif // OSCILLOSCOPEFORM_H
