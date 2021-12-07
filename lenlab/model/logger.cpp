#include "logger.h"

#include <QDebug>

#include "lenlab_protocol.h"

namespace model {

const QStringList Logger::names = {
    "Kanal 1",
    "Kanal 2",
    "Kanal 3",
    "Kanal 4"
};

Logger::Logger(protocol::Board* board)
    : Component{board}
    , waveform{new Waveform(this)}
{
    connect(board, &protocol::Board::setup,
            this, &Logger::setup);

    connect(board, &protocol::Board::reply,
            this, &Logger::reply);

    connect(board, &protocol::Board::error,
            this, &Logger::error);
}

const QStringList& Logger::channel_names()
{
    return names;
    /*
    QVector< QString > labels;

    for (int i = 0; i < 4; ++i) {
        labels.append(QString("Kanal ") + QString::number(i + 1));
    }

    return labels;
    */
}

void Logger::setup(std::shared_ptr< usb::Packet >& packet)
{
    qDebug() << "setup";

    waveform->deleteLater();
    waveform = new Waveform(this);
    waveform->interval = 256;

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

void Logger::reply(std::shared_ptr< usb::Packet >& packet)
{
    qDebug() << "reply";
    if (packet->buffer[0] == Log) {
        qDebug() << "Log" << packet->buffer[4];

        uint32_t* payload = (uint32_t*) packet->buffer + 1;
        float x = (float) (payload[0] * waveform->interval) / 1000.0;
        float y[4];
        for (int i = 0; i < 4; ++i) {
            y[i] = (float) payload[1 + i] / 4096.0 * 3.3;
        }
        waveform->append(x, y, 4);

        if (packet->buffer[4] == 10) {
            auto command = std::make_shared< usb::Packet >();
            command->buffer[0] = stopLogger;
            command->buffer[1] = nullType;
            command->buffer[2] = 0;
            command->buffer[3] = 0;
            command->length = 4;
            board->command(command);

            waveform->csv(std::cerr);
            emit newWaveform(waveform);
        }
    }
}

void Logger::error()
{

}


} // namespace model
