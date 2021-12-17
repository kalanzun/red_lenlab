#ifndef MODEL_OSCILLOSCOPE_H
#define MODEL_OSCILLOSCOPE_H

#include "component.h"
#include "parameter.h"

#include "protocol/message.h"

namespace model {

struct Page {
    struct Head head;
    uint16_t log2oversamples;
    uint16_t trigger_config;
    uint16_t channel;
    uint16_t index;
    uint16_t trigger;
    uint16_t res[5];
    uint16_t values[500];
};

class Oscilloscope : public Component
{
    Q_OBJECT

    static const std::array< int, 3 > timerange_values;

public:
    static const Parameter samplerate;
    static const Parameter timerange;
    static const Parameter valuerange;

    explicit Oscilloscope(protocol::Board* board);

    void start();
    void stop();

public slots:
    void setup(const std::shared_ptr< protocol::Message >& message);
    void reply(const std::shared_ptr< protocol::Message >& message);
    void error();

private:
    std::shared_ptr< model::Waveform > incoming;

    void setupWaveform();
};

} // namespace model

#endif // MODEL_OSCILLOSCOPE_H
