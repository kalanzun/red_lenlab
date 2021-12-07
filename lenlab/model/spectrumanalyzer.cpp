#include "spectrumanalyzer.h"

namespace model {

SpectrumAnalyzer::SpectrumAnalyzer(protocol::Board* board)
    : Component{board}
    , board{board}
{

}

} // namespace model
