#ifndef MODEL_OSCILLOSCOPE_H
#define MODEL_OSCILLOSCOPE_H

#include <QObject>

#include "protocol/board.h"

namespace model {

class Oscilloscope : public QObject
{
    Q_OBJECT

    protocol::Board* board;

public:
    explicit Oscilloscope(protocol::Board* board);

signals:

};

} // namespace model

#endif // MODEL_OSCILLOSCOPE_H
