#include "virtualdevice.h"

#include "message.h"

namespace protocol {

VirtualDevice::VirtualDevice(QObject *parent)
    : Device{parent}
{

}

void VirtualDevice::send(std::shared_ptr< Message >& message)
{

}

} // namespace protocol
