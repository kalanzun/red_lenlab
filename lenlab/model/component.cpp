#include "component.h"

#include "protocol/board.h"

namespace model {

Component::Component(protocol::Board* board)
    : QObject{board}
    , board{board}
{

}

const QString Component::y2_label() const
{
    return QString();
}

} // namespace model
