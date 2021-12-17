#include "component.h"

#include "protocol/board.h"
#include "model/waveform.h"

namespace model {

Component::Component(protocol::Board* board)
    : QObject{board}
    , board{board}
    , waveform{std::make_shared< model::Waveform >()}
{

}

model::Waveform* Component::getWaveform() const
{
    return waveform.get();
}

} // namespace model
