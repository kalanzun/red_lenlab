#include "component.h"

#include "protocol/board.h"

namespace model {

Component::Component(protocol::Board* board)
    : QObject{board}
    , board{board}
{

}

} // namespace model
