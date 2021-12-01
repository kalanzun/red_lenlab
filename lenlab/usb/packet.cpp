#include "packet.h"

namespace usb {

Packet::Packet()
{

}

QDebug operator<<(QDebug debug, const std::shared_ptr< Packet >& packet)
{
    //QDebugStateSaver saver(debug);
    debug.nospace() << "Packet[" << packet->length << "](" << packet->buffer[0] << ", " << packet->buffer[1] << ", " << (packet->buffer[2] + (packet->buffer[3] << 8)) << ")";
    return debug;
}

} // namespace usb
