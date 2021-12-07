#ifndef MODEL_OSCILLOSCOPE_H
#define MODEL_OSCILLOSCOPE_H

#include "component.h"

#include "protocol/board.h"

namespace model {

class Oscilloscope : public Component
{
    Q_OBJECT

public:
    explicit Oscilloscope(protocol::Board* board);

    QVector< QString > channel_names();

signals:

};

} // namespace model

#endif // MODEL_OSCILLOSCOPE_H
