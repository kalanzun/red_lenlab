#ifndef MODEL_LOGGER_H
#define MODEL_LOGGER_H

#include "component.h"

namespace protocol { class Message; }

namespace model {

class Logger : public Component
{
    Q_OBJECT

public:
    explicit Logger(protocol::Board* board);

    void start();
    void stop();
    void reset();

public slots:
    void setup(std::shared_ptr< protocol::Message >& message);
    void reply(std::shared_ptr< protocol::Message >& message);
    void error();
};

} // namespace model

#endif // MODEL_LOGGER_H
