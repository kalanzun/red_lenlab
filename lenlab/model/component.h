#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include <QObject>

namespace protocol { class Board; }

namespace model {

class Waveform;

class Component : public QObject
{
    Q_OBJECT

protected:
    protocol::Board* board;

    bool running = false;

public:
    model::Waveform* waveform;

    explicit Component(protocol::Board* board);

signals:
    void WaveformCreated(model::Waveform* waveform);
};

} // namespace model

#endif // MODEL_COMPONENT_H
