#ifndef OSCILLOSCOPEFORM_H
#define OSCILLOSCOPEFORM_H

#include <QWidget>

namespace model {

class Lenlab;

}

namespace app {

namespace Ui {
class OscilloscopeForm;
}

class OscilloscopeForm : public QWidget
{
    Q_OBJECT

    model::Lenlab* lenlab;

public:
    explicit OscilloscopeForm(QWidget *parent = nullptr);
    ~OscilloscopeForm();

    void setModel(model::Lenlab* lenlab);

private:
    Ui::OscilloscopeForm *ui;
};

} // namespace app
#endif // OSCILLOSCOPEFORM_H
