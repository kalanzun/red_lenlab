#include "lenlab.h"

namespace model {

Lenlab::Lenlab(QObject *parent)
    : QObject{parent}
    , board{new protocol::Board(this)}
    , logger{new Logger(board)}
{

}

void Lenlab::lookForDevice()
{
    board->lookForDevice();
}

} // namespace protocol
