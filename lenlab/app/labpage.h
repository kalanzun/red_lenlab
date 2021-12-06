#ifndef LABPAGE_H
#define LABPAGE_H

#include <QWidget>

#include "labchart.h"

namespace app {

namespace Ui {
class LabPage;
}

class LabPage : public QWidget
{
    Q_OBJECT

public:
    explicit LabPage(QWidget *parent = nullptr);
    ~LabPage();

    QWidget* contents();
    LabChart* lab_chart();

private:
    Ui::LabPage *ui;
};

} // namespace app
#endif // LABPAGE_H
