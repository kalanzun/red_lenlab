#include "lenlab.h"

namespace model {

Lenlab::Lenlab(QObject *parent)
    : QObject{parent}
    , board{new protocol::Board(this)}
    , logger{new Logger(board)}
    , oscilloscope{new Oscilloscope(board)}
    , frequency{new FrequencySweep(board)}
{

}

void Lenlab::lookForDevice()
{
    board->lookForDevice();
}

} // namespace protocol
