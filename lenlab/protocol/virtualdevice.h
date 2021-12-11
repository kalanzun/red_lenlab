#ifndef PROTOCOL_VIRTUALDEVICE_H
#define PROTOCOL_VIRTUALDEVICE_H

#include "device.h"

namespace protocol {

class VirtualDevice : public Device
{
    Q_OBJECT

public:
    explicit VirtualDevice(QObject *parent = nullptr);

public slots:
    void send(std::shared_ptr< Message >& message);
};

} // namespace protocol

#endif // PROTOCOL_VIRTUALDEVICE_H
