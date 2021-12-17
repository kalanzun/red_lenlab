#include "oscilloscope.h"

#include "protocol/board.h"
#include "waveform.h"

namespace model {

const std::array< int, 3 > Oscilloscope::timerange_values{2, 4, 5};

const Parameter Oscilloscope::samplerate{
    4,
    [](int index) { return 1'000 >> index; },
    "%1 kHz"
};

const Parameter Oscilloscope::timerange{
    timerange_values.size(),
    [](int index) { return timerange_values[index]; },
    "%1 ms"
};

const Parameter Oscilloscope::valuerange{
    2,
    [](int index) { return 1650 / (index + 1); },
    "%1 mV"
};

Oscilloscope::Oscilloscope(protocol::Board* board)
    : Component{board}
{
    connect(board, &protocol::Board::setup,
            this, &Oscilloscope::setup);

    connect(board, &protocol::Board::reply,
            this, &Oscilloscope::reply);

    connect(board, &protocol::Board::error,
            this, &Oscilloscope::error);
}

void Oscilloscope::start()
{
    waveform->deleteLater();
    waveform = new Waveform{this};

    auto start_logger = protocol::Message::createCommand(startOscilloscope, IntArray);
    start_logger->addInt(waveform->getInterval());
    board->send(start_logger);
}

void Oscilloscope::reset()
{
}

void Oscilloscope::setup(const std::shared_ptr< protocol::Message >& message)
{
    auto set_signal_sine = protocol::Message::createCommand(setSignalSine, IntArray);
    set_signal_sine->addInt(8); // mutliplier
    set_signal_sine->addInt(2); // predivider
    set_signal_sine->addInt(9); // divider
    set_signal_sine->addInt(static_cast< uint32_t >(std::round((1<<11) * 1.65 / 1.65))); // amplitude
    set_signal_sine->addInt(0); // second
    board->send(set_signal_sine);
}

void Oscilloscope::reply(const std::shared_ptr< protocol::Message >& message)
{
    if (message->head->reply == OscilloscopeData) {
        auto page = (struct Page*) message->getBuffer();

        for (auto i = 0; i < 500; ++i) {
            if (page->channel == 0) waveform->x.append((float) page->index * 500 + i);
            waveform->y[page->channel].append((float) page->values[i] / 4096.0 * 3.3 - 1.65);
        }

        if (page->index == 11 && page->channel == 1) {
            emit WaveformCreated(waveform);
        }
    }
}

void Oscilloscope::error()
{
}

} // namespace model
