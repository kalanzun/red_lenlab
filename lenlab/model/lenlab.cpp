#include "lenlab.h"

#include "protocol/board.h"
#include "logger.h"
#include "oscilloscope.h"
#include "spectrumanalyzer.h"

namespace model {

Lenlab::Lenlab(QObject *parent)
    : QObject{parent}
    , board{new protocol::Board{this}}
    , logger{new Logger{board}}
    , oscilloscope{new Oscilloscope{board}}
    , spectrum{new SpectrumAnalyzer{board}}
{

}

void Lenlab::lookForDevice(bool create_virtual_device)
{
    board->lookForDevice(create_virtual_device);
}

} // namespace protocol
