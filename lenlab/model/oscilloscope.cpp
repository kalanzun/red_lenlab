#include "oscilloscope.h"

#include "protocol/board.h"

namespace model {

const QStringList Oscilloscope::names = {
    "Kanal 1",
    "Kanal 2"
};

Oscilloscope::Oscilloscope(protocol::Board* board)
    : Component{board}
{

}

const QString Oscilloscope::x_label() const
{
    return QString("Zeit [ms]");
}

const QString Oscilloscope::y_label() const
{
    return QString("Spannung [V]");
}

const QStringList& Oscilloscope::channel_names() const
{
    return names;
    /*
    QVector< QString > labels;

    for (int i = 0; i < 2; ++i) {
        labels.append(QString("Kanal ") + QString::number(i + 1));
    }

    return labels;
    */
}

} // namespace model
