#ifndef GUI_OSCILLOSCOPEFORM_H
#define GUI_OSCILLOSCOPEFORM_H

#include <QWidget>

namespace gui {

namespace Ui {
class OscilloscopeForm;
}

class OscilloscopeForm : public QWidget
{
    Q_OBJECT

public:
    explicit OscilloscopeForm(QWidget *parent = 0);
    ~OscilloscopeForm();

private:
    Ui::OscilloscopeForm *ui;
};


} // namespace gui
#endif // GUI_OSCILLOSCOPEFORM_H
