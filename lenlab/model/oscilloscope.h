#ifndef MODEL_OSCILLOSCOPE_H
#define MODEL_OSCILLOSCOPE_H

#include "component.h"

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

public:
    explicit Oscilloscope(protocol::Board* board);

    void start();
    void stop();
    void reset();

public slots:
    void setup(std::shared_ptr< protocol::Message >& message);
    void reply(std::shared_ptr< protocol::Message >& message);
    void error();
};

} // namespace model

#endif // MODEL_OSCILLOSCOPE_H
