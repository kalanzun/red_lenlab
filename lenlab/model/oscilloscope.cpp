#include "oscilloscope.h"

#include "protocol/board.h"

namespace model {

Oscilloscope::Oscilloscope(protocol::Board* board)
    : Component{board}
{

}

} // namespace model
