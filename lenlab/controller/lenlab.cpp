#include "lenlab.h"

namespace controller {

Lenlab::Lenlab(QObject *parent)
    : QObject{parent}
    , manager{new Manager(this)}
    , logger{new Logger(manager)}
{

}

void Lenlab::lookForBoard()
{
    manager->lookForBoard();
}

} // namespace protocol
