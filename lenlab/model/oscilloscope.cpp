#include "oscilloscope.h"

namespace model {

Oscilloscope::Oscilloscope(protocol::Board* board)
    : QObject{board}
    , board{board}
{

}

} // namespace model
