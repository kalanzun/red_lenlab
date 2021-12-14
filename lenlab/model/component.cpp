#include "component.h"

#include "protocol/board.h"
#include "model/waveform.h"

namespace model {

Component::Component(protocol::Board* board)
    : QObject{board}
    , board{board}
    , waveform{new model::Waveform{this}}
{

}

model::Waveform* Component::getWaveform() const
{
    return waveform;
}

} // namespace model
