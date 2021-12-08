#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include <QObject>

namespace protocol {

class Board;

}

namespace model {

class Waveform;

class Component : public QObject
{
    Q_OBJECT

protected:
    protocol::Board* board;
    model::Waveform* waveform;

public:
    explicit Component(protocol::Board* board);

    model::Waveform* getWaveform();

signals:
    void WaveformCreated(model::Waveform* waveform);
};

} // namespace model

#endif // MODEL_COMPONENT_H
