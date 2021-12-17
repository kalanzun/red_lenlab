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
    setupWaveform();

    connect(board, &protocol::Board::setup,
            this, &Oscilloscope::setup);

    connect(board, &protocol::Board::reply,
            this, &Oscilloscope::reply);

    connect(board, &protocol::Board::error,
            this, &Oscilloscope::error);
}

void Oscilloscope::start()
{
    if (running) return;
    running = true;

    incoming = std::make_shared< model::Waveform >();

    auto cmd = protocol::Message::createCommand(startOscilloscope, IntArray);
    cmd->addInt(samplerate.getIndex());
    board->send(cmd);
}

void Oscilloscope::stop()
{
    if (!running) return;
    running = false;
}

void Oscilloscope::setup(const std::shared_ptr< protocol::Message >& message)
{
    running = false;

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
    if (!running) return;

    if (message->head->reply == OscilloscopeData) {
        auto page = (struct Page*) message->getBuffer();

        for (auto i = 0; i < 500; ++i) {
            if (page->channel == 0) incoming->x.append((page->index * 500 + i) / 1e3f); // ms
            incoming->y[page->channel].append(page->values[i] / 4096.f * 3.3f - 1.65f); // V
        }

        if (page->index == 11 && page->channel == 1) {
            waveform.swap(incoming);
            setupWaveform();
            emit WaveformCreated(waveform.get());
        }
    }
}

void Oscilloscope::error()
{
    running = false;
}

void Oscilloscope::setupWaveform()
{
    waveform->x_range.a = -2.f;
    waveform->x_range.b = 2.f;
    waveform->y_range.a = -1.65f;
    waveform->y_range.b = 1.65f;
}

} // namespace model
