#ifndef MODEL_LOGGER_H
#define MODEL_LOGGER_H

#include "component.h"

namespace usb {

class Packet;

}

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
    void setup(std::shared_ptr< usb::Packet >& packet);
    void reply(std::shared_ptr< usb::Packet >& packet);
    void error();
};

} // namespace model

#endif // MODEL_LOGGER_H
