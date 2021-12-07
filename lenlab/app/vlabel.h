#ifndef APP_VLABEL_H
#define APP_VLABEL_H

#include <QLabel>

namespace app {

class VLabel : public QLabel
{
    Q_OBJECT

public:
    explicit VLabel(QWidget *parent);
    explicit VLabel(const QString &text, QWidget *parent);

protected:
    void paintEvent(QPaintEvent*);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
};

} // namespace app

#endif // APP_VLABEL_H
