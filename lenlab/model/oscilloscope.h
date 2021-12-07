#ifndef MODEL_OSCILLOSCOPE_H
#define MODEL_OSCILLOSCOPE_H

#include "component.h"

namespace model {

class Oscilloscope : public Component
{
    Q_OBJECT

    static const QStringList names;

public:
    explicit Oscilloscope(protocol::Board* board);

    const QStringList& channel_names();

signals:

};

} // namespace model

#endif // MODEL_OSCILLOSCOPE_H
