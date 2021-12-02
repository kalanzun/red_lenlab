#include "logger.h"

#include <QDebug>

#include "lenlab_protocol.h"

namespace model {

Logger::Logger(protocol::Board* board)
    : QObject{board}
    , board{board}
{
    connect(board, &protocol::Board::setup,
            this, &Logger::setup);

    connect(board, &protocol::Board::reply,
            this, &Logger::reply);

    connect(board, &protocol::Board::teardown,
            this, &Logger::teardown);
}

void Logger::setup(std::shared_ptr< usb::Packet > packet)
{
    qDebug() << "setup";
    // TODO Message helpers
    auto command = std::make_shared< usb::Packet >();
    command->buffer[0] = startLogger;
    command->buffer[1] = IntArray;
    command->buffer[2] = 0;
    command->buffer[3] = 0;
    command->buffer[4] = 0; // interval in ms
    command->buffer[5] = 4; // 1024
    command->buffer[6] = 0;
    command->buffer[7] = 0;
    command->length = 8;
    board->command(std::move(command));
}

void Logger::reply(std::shared_ptr< usb::Packet > packet)
{
    qDebug() << "reply";
    if (packet->buffer[0] == Log) {
        qDebug() << "Log" << packet->buffer[4];

        if (packet->buffer[4] == 10) {
            auto command = std::make_shared< usb::Packet >();
            command->buffer[0] = stopLogger;
            command->buffer[1] = nullType;
            command->buffer[2] = 0;
            command->buffer[3] = 0;
            command->length = 4;
            board->command(std::move(command));
        }
    }
}

void Logger::teardown()
{

}


} // namespace model
