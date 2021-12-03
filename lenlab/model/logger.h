#ifndef MODEL_LOGGER_H
#define MODEL_LOGGER_H

#include <QObject>
#include <QSharedPointer>

#include "protocol/board.h"
#include "waveform.h"

namespace model {

class Logger : public QObject
{
    Q_OBJECT

    protocol::Board* board;
    QSharedPointer< Waveform > waveform;

public:
    explicit Logger(protocol::Board* board);

signals:
    void newWaveform(QSharedPointer< Waveform >& waveform);

public slots:
    void setup(std::shared_ptr< usb::Packet > packet);
    void reply(std::shared_ptr< usb::Packet > packet);
    void teardown();
};

} // namespace model

#endif // MODEL_LOGGER_H
