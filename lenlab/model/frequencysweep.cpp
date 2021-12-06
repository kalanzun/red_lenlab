#include "frequencysweep.h"

namespace model {

FrequencySweep::FrequencySweep(protocol::Board* board)
    : QObject{board}
    , board{board}
{

}

} // namespace model
