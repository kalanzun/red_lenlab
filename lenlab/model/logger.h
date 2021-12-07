#ifndef MODEL_LOGGER_H
#define MODEL_LOGGER_H

#include "component.h"

namespace usb {

class Packet;

}

namespace model {

class Waveform;

class Logger : public Component
{
    Q_OBJECT

    Waveform* waveform;

public:
    explicit Logger(protocol::Board* board);

signals:
    void newWaveform(Waveform* waveform);

public slots:
    void setup(std::shared_ptr< usb::Packet >& packet);
    void reply(std::shared_ptr< usb::Packet >& packet);
    void error();
};

} // namespace model

#endif // MODEL_LOGGER_H
