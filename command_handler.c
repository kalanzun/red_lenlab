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


void
CommandHandlerInit(tCommandHandler *self)
{
    PacketQueueInit(&self->command_queue);
}


void
CommandHandlerMain(tCommandHandler *self)
{
    tPacket *packet;

    packet = PacketQueueRead(&self->command_queue);
    if (packet) {
        packet->payload[packet->size] = 0;
        DEBUG_PRINT("%i, %s\n", packet->size, packet->payload);
    }

}
