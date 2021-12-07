#ifndef MODEL_OSCILLOSCOPE_H
#define MODEL_OSCILLOSCOPE_H

#include "component.h"

namespace model {

class Oscilloscope : public Component
{
    Q_OBJECT

public:
    explicit Oscilloscope(protocol::Board* board);

signals:

};

} // namespace model

#endif // MODEL_OSCILLOSCOPE_H
