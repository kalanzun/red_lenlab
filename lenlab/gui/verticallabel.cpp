#include "verticallabel.h"

#include <QPainter>

VerticalLabel::VerticalLabel(QWidget *parent)
    : QLabel(parent)
{
}

VerticalLabel::VerticalLabel(const QString &text, QWidget *parent)
: QLabel(text, parent)
{
}

void
VerticalLabel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    // painter.translate(.width(), 0);
    // painter.rotate(90);
    painter.translate(0, height());
    painter.rotate(270);
    painter.drawText(0, 0, height(), width(), alignment(), text());
}

QSize
VerticalLabel::minimumSizeHint() const
{
    QSize s = QLabel::minimumSizeHint();
    return QSize(s.height(), s.width());
}

QSize
VerticalLabel::sizeHint() const
{
    QSize s = QLabel::sizeHint();
    return QSize(s.height(), s.width());
}
