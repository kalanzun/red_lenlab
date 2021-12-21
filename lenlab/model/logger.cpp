#include "logger.h"

#include <QDebug>

#include "protocol/board.h"
#include "protocol/message.h"
#include "waveform.h"

namespace model {

const std::array< int, 6 > Logger::interval_values{100, 200, 500, 1000, 2000, 5000};

Logger::Logger(protocol::Board* board)
    : Component{board}
{
    interval = new Parameter{
        interval_values.size(),
        [](int index) { return interval_values[index]; },
        "%1 ms",
        3,
        this
    };

    setupWaveform();

    connect(board, &protocol::Board::setup,
            this, &Logger::setup);

    connect(board, &protocol::Board::reply,
            this, &Logger::reply);

    connect(board, &protocol::Board::error,
            this, &Logger::error);
}

void Logger::start()
{
    if (running) return;
    running = true;

    if (!waveform->is_locked()) {
        waveform->setInterval(interval->getValue());
        waveform->setLocked();
    }

    auto cmd = protocol::Message::createCommand(startLogger, IntArray);
    cmd->addInt(waveform->getInterval());
    board->send(cmd);
}

void Logger::stop()
{
    if (!running) return;
    running = false;

    auto cmd = protocol::Message::createCommand(stopLogger);
    board->send(cmd);
}

void Logger::reset()
{
    if (running) stop();

    waveform->deleteLater();
    waveform = new Waveform{this};
    setupWaveform();
    emit WaveformCreated(waveform);
}

void Logger::setup(const std::shared_ptr< protocol::Message >& message)
{
    running = false;
}

void Logger::reply(const std::shared_ptr< protocol::Message >& message)
{
    if (!running) return;
    assert(waveform->is_locked());

    if (message->head->reply == Log) {
        struct Sample sample;
        sample.x = (waveform->getLength() * waveform->getInterval()) / 1000.f;
        sample.x_range.a = 0.f;
        sample.x_range.b = (waveform->round_up_towards(40, waveform->getLength()) * waveform->getInterval()) / 1000.f;
        for (auto i = 0; i < sample.channels; ++i) {
            sample.y[i] = message->getInt(i + 1) / 4096.f * 3.3f;
        }
        waveform->appendSample(sample);
    }
}

void Logger::error()
{
    running = false;
}

void Logger::setupWaveform()
{
    waveform->x_range.b = 40.f;
    waveform->y_range.b = 3.3f;
}

} // namespace model
