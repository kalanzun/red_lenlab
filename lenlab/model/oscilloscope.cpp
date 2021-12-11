#include "oscilloscope.h"

#include <QDebug>

#include "protocol/board.h"
#include "waveform.h"

namespace model {

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
    waveform = new Waveform(this);

    auto start_logger = protocol::Message::createCommand(startOscilloscope, IntArray);
    start_logger->addInt(waveform->interval);
    board->command(start_logger);
}

void Oscilloscope::reset()
{
}

void Oscilloscope::setup(std::shared_ptr< protocol::Message >& message)
{
    qDebug() << "setup";

    auto set_signal_sine = protocol::Message::createCommand(setSignalSine, IntArray);
    set_signal_sine->addInt(8); // mutliplier
    set_signal_sine->addInt(2); // predivider
    set_signal_sine->addInt(9); // divider
    set_signal_sine->addInt(static_cast< uint32_t >(std::round((1<<11) * 1.65 / 1.65))); // amplitude
    set_signal_sine->addInt(0); // second
    board->command(set_signal_sine);
}

void Oscilloscope::reply(std::shared_ptr< protocol::Message >& message)
{
    if (message->head->reply == OscilloscopeData) {
        struct Page* page = (struct Page*) message->getBuffer();

        qDebug() << "OscilloscopeData" << page->channel << page->index;

        for (int i = 0; i < 500; ++i) {
            if (page->channel == 0) waveform->x_values.append((float) page->index * 500 + i);
            waveform->y_values[page->channel].append((float) page->values[i] / 4096.0 * 3.3 - 1.65);
        }

        if (page->index == 11 && page->channel == 1) {
            qDebug() << "emit WaveformCreated";
            emit WaveformCreated(waveform);
        }
    }

    if (message->head->reply == SignalSine) {
        qDebug() << "SignalSine";
    }
}

void Oscilloscope::error()
{
    qDebug() << "error";
}

} // namespace model
