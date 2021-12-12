#ifndef PROTOCOL_VIRTUALDEVICE_H
#define PROTOCOL_VIRTUALDEVICE_H

#include "device.h"

namespace protocol {

class VirtualDevice : public Device
{
    Q_OBJECT

public:
    explicit VirtualDevice(QObject *parent = nullptr);

signals:
    void command(std::shared_ptr< Message >& message);

public slots:
    void send(std::shared_ptr< Message >& message);

private slots:
    void handleCommand(std::shared_ptr< Message >& cmd);
};

} // namespace protocol

#endif // PROTOCOL_VIRTUALDEVICE_H
