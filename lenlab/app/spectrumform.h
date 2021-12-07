#ifndef APP_SPECTRUMFORM_H
#define APP_SPECTRUMFORM_H

#include <QWidget>

namespace model {

class Lenlab;

}

namespace app {

namespace Ui {
class SpectrumForm;
}

class SpectrumForm : public QWidget
{
    Q_OBJECT

    model::Lenlab* lenlab;

public:
    explicit SpectrumForm(QWidget *parent = nullptr);
    ~SpectrumForm();

    void setModel(model::Lenlab* lenlab);

    void setupChart() const;

private:
    Ui::SpectrumForm *ui;
};


} // namespace app
#endif // APP_SPECTRUMFORM_H
