#include "oscilloscope.h"

namespace model {

Oscilloscope::Oscilloscope(protocol::Board* board)
    : Component{board}
{

}

QVector< QString > Oscilloscope::channel_names()
{
    QVector< QString > labels;

    for (int i = 0; i < 2; ++i) {
        labels.append(QString("Kanal ") + QString::number(i + 1));
    }

    return labels;
}

} // namespace model
