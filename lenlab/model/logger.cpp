#include "logger.h"

#include <QDebug>

#include "protocol/board.h"
#include "protocol/message.h"
#include "waveform.h"

namespace model {

const std::array< int, 6 > Logger::interval_values{100, 200, 500, 1000, 2000, 5000};

const Parameter Logger::interval{
    interval_values.size(),
    [](int index) { return interval_values[index]; },
    "%1 ms",
    3
};

Logger::Logger(protocol::Board* board)
    : Component{board}
{
    waveform->interval = 256;

    connect(board, &protocol::Board::setup,
            this, &Logger::setup);

    connect(board, &protocol::Board::reply,
            this, &Logger::reply);

    connect(board, &protocol::Board::error,
            this, &Logger::error);
}

void Logger::start()
{
    auto start_logger = protocol::Message::createCommand(startLogger, IntArray);
    start_logger->addInt(waveform->interval);
    board->send(start_logger);
}

void Logger::stop()
{
    auto stop_logger = protocol::Message::createCommand(stopLogger);
    board->send(stop_logger);
}

void Logger::reset()
{
    auto interval = waveform->interval;
    waveform->deleteLater();

    waveform = new Waveform{this};
    waveform->interval = interval;
    emit WaveformCreated(waveform);
}

void Logger::setup(const std::shared_ptr< protocol::Message >& message)
{
    qDebug() << "Logger::setup";
}

void Logger::reply(const std::shared_ptr< protocol::Message >& message)
{
    if (message->head->reply == Log) {
        qDebug() << "Log" << message->getInt(0);

        struct Sample sample;
        sample.x = (float) (message->getInt(1) * waveform->interval) / 1000.0;
        for (auto i = 0; i < sample.channels; ++i) {
            sample.y[i] = (float) message->getInt(i + 1) / 4096.0 * 3.3;
        }
        waveform->appendSample(sample);
    }
}

void Logger::error()
{
    qDebug() << "Logger::error";
}

void Logger::setIntervalIndex(int index)
{
    qDebug() << "setIntervalIndex";
    waveform->interval = interval.value(index);
}

} // namespace model
