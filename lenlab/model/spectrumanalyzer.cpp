#include "spectrumanalyzer.h"

namespace model {

SpectrumAnalyzer::SpectrumAnalyzer(protocol::Board* board)
    : QObject{board}
    , board{board}
{

}

} // namespace model
