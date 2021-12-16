#ifndef MODEL_LOGGER_H
#define MODEL_LOGGER_H

#include "component.h"
#include "parameter.h"

namespace protocol { class Message; }

namespace model {

class Logger : public Component
{
    Q_OBJECT

    static const std::array< const int, 6 > interval_values;

public:
    static const Parameter interval;

    explicit Logger(protocol::Board* board);

    void start();
    void stop();
    void reset();

public slots:
    void setup(const std::shared_ptr< protocol::Message >& message);
    void reply(const std::shared_ptr< protocol::Message >& message);
    void error();
};

} // namespace model

#endif // MODEL_LOGGER_H
