#include "vlabel.h"

#include <QPainter>

namespace app {

VLabel::VLabel(QWidget *parent)
    : QLabel{parent}
{

}

VLabel::VLabel(const QString &text, QWidget *parent)
    : QLabel{text, parent}
{

}

void
VLabel::paintEvent(QPaintEvent*)
{
    QPainter painter{this};
    painter.translate(0, height());
    painter.rotate(270);
    painter.drawText(0, 0, height(), width(), alignment(), text());
}

QSize
VLabel::minimumSizeHint() const
{
    auto s = QLabel::minimumSizeHint();
    return QSize{s.height(), s.width()};
}

QSize
VLabel::sizeHint() const
{
    auto s = QLabel::sizeHint();
    return QSize{s.height(), s.width()};
}

} // namespace app
