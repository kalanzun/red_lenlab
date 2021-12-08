#include "logger.h"

#include <QDebug>

#include "lenlab_protocol.h"
#include "protocol/board.h"
#include "usb/packet.h"
#include "waveform.h"

namespace model {

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
    // TODO Message helpers
    auto command = std::make_shared< usb::Packet >();
    command->buffer[0] = startLogger;
    command->buffer[1] = IntArray;
    command->buffer[2] = 0;
    command->buffer[3] = 0;
    command->buffer[4] = 0; // interval in ms
    command->buffer[5] = 1; // 256
    command->buffer[6] = 0;
    command->buffer[7] = 0;
    command->length = 8;
    board->command(command);
}

void Logger::stop()
{
    auto command = std::make_shared< usb::Packet >();
    command->buffer[0] = stopLogger;
    command->buffer[1] = nullType;
    command->buffer[2] = 0;
    command->buffer[3] = 0;
    command->length = 4;
    board->command(command);
}

void Logger::reset()
{
    waveform->deleteLater();
    waveform = new Waveform(this);
    waveform->interval = 256;
    emit WaveformCreated(waveform);
}

void Logger::setup(std::shared_ptr< usb::Packet >& packet)
{
    qDebug() << "setup";
}

void Logger::reply(std::shared_ptr< usb::Packet >& packet)
{
    qDebug() << "reply";
    if (packet->buffer[0] == Log) {
        qDebug() << "Log" << packet->buffer[4];

        uint32_t* payload = (uint32_t*) packet->buffer + 1;
        struct Sample sample;
        sample.x = (float) (payload[0] * waveform->interval) / 1000.0;
        for (int i = 0; i < sample.channels; ++i) {
            sample.y[i] = (float) payload[1 + i] / 4096.0 * 3.3;
        }
        waveform->appendSample(sample);
    }
}

void Logger::error()
{
    qDebug() << "error";
}

} // namespace model
