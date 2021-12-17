#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include <memory>

#include <QObject>

namespace protocol { class Board; }

namespace model {

class Waveform;

class Component : public QObject
{
    Q_OBJECT

protected:
    protocol::Board* board;
    std::shared_ptr< model::Waveform > waveform;

    bool running = false;

public:
    explicit Component(protocol::Board* board);

    model::Waveform* getWaveform() const;

signals:
    void WaveformCreated(model::Waveform* waveform);
};

} // namespace model

#endif // MODEL_COMPONENT_H
