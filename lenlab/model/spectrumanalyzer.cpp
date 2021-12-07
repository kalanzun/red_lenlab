#include "spectrumanalyzer.h"

#include "protocol/board.h"

namespace model {

SpectrumAnalyzer::SpectrumAnalyzer(protocol::Board* board)
    : Component{board}
{

}

} // namespace model
