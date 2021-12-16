#ifndef MODEL_SIGNALGENERATOR_H
#define MODEL_SIGNALGENERATOR_H

#include <QObject>

#include "parameter.h"

namespace protocol { class Board; }

namespace model {

class SignalGenerator : public QObject
{
    Q_OBJECT

    static const std::array< std::array< uint8_t, 3 >, 130 > frequency_values;

public:
    static const Parameter amplitude;
    static const Parameter frequency;
    static const Parameter multiplier;

    explicit SignalGenerator(protocol::Board* board);
};

} // namespace model

#endif // MODEL_SIGNALGENERATOR_H
