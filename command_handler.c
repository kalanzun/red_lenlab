/*
 * command_handler.c
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#include <stdbool.h>
#include <stdint.h>
#include "debug.h"
#include "command_handler.h"
#include "reply_handler.h"


void
CommandHandlerInit(tCommandHandler *self)
{
    PacketQueueInit(&self->command_queue);
}

void
NoOperation (tCommandHandler *self, tPacket *command)
{

}

#define min(a, b) (((a) < (b))? (a) : (b))

inline void
SendMessage(tPacketQueue *queue, const uint8_t data[], uint32_t size)
{
    tPacket *packet;
    uint32_t i;

    packet = PacketQueueWrite(queue);
    packet->size = min(size + 1, PACKET_QUEUE_PAYLOAD_SIZE);

    packet->payload[0] = 1; // Reply Code for Message
    for (i = 1; i < packet->size; i++)
        packet->payload[i] = data[i-1];

    PacketQueueWriteDone(queue);

}

const uint8_t name[] = "KIT Lenlab Red Firmware version 0.1 year 2017";

#define NAME_SIZE (sizeof(name))

void
GetName (tCommandHandler *self, tPacket *command)
{
    DEBUG_PRINT("GetName()\n");
    if (!PacketQueueFull(self->reply_queue)) {
        SendMessage(self->reply_queue, name, NAME_SIZE);
    }
}

typedef void (* const tCommandFunction)(tCommandHandler *, tPacket *);

tCommandFunction commands[] =
{
    NoOperation,
    GetName
};

#define NUM_COMMANDS (sizeof(commands) / sizeof(tCommandFunction))


void
CommandHandlerMain(tCommandHandler *self)
{
    tPacket *packet;
    uint8_t command;

    if (!PacketQueueEmpty(&self->command_queue)) {
        packet = PacketQueueRead(&self->command_queue);
        command = packet->payload[0];
        if (command < NUM_COMMANDS)
            commands[command](self, packet);
        PacketQueueReadDone(&self->command_queue);
    }

}
