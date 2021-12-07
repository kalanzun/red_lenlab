#include "component.h"

namespace model {

Component::Component(protocol::Board* board)
    : QObject{board}
    , board{board}
{

}

} // namespace model
