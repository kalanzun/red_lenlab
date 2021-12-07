#ifndef MODEL_LOGGER_H
#define MODEL_LOGGER_H

#include "component.h"

#include <QSharedPointer>

#include "protocol/board.h"
#include "waveform.h"

namespace model {

class Logger : public Component
{
    Q_OBJECT

    Waveform* waveform;

public:
    explicit Logger(protocol::Board* board);

    QVector< QString > channel_names();

signals:
    void newWaveform(Waveform* waveform);

public slots:
    void setup(std::shared_ptr< usb::Packet >& packet);
    void reply(std::shared_ptr< usb::Packet >& packet);
    void error();
};

} // namespace model

#endif // MODEL_LOGGER_H
