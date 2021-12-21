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
    Parameter* amplitude;
    Parameter* frequency;
    Parameter* multiplier;

    explicit SignalGenerator(protocol::Board* board);
};

} // namespace model

#endif // MODEL_SIGNALGENERATOR_H
