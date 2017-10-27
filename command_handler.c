/*
 * command_handler.c
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#include <stdbool.h>
#include <stdint.h>
#include "utils/ustdlib.h"
#include "debug.h"
#include "command_handler.h"
#include "usb_device.h"
#include "adc.h"


tCommandHandler command_handler;


void
NoOperation(tEvent *event)
{

}

//#define min(a, b) (((a) < (b))? (a) : (b))

/*

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
    DEBUG_PRINT("GetName\n");
    if (!PacketQueueFull(self->reply_queue)) {
        SendMessage(self->reply_queue, name, NAME_SIZE);
    }
}


void
SetLoggerTimestep(tCommandHandler *self, tPacket *command)
{
    tPacket *packet;
    uint32_t timestep;

    if (command->size < 5) return;

    timestep = *(uint32_t *) (command->payload+1);
    DEBUG_PRINT("SetLoggerTimestamp %i\n", timestep);
    // set timestep
    if (!PacketQueueFull(self->reply_queue)) {
        packet = PacketQueueWrite(self->reply_queue);
        packet->payload[0] = 2; // Reply Code for OK
        packet->size = usnprintf((char *) packet->payload+1, PACKET_QUEUE_PAYLOAD_SIZE-1, "SetLoggerTimestep %i", timestep) + 1 + 1; // reply code and string terminator

        PacketQueueWriteDone(self->reply_queue);
    }

}
void
GetADCInterruptCounter(tCommandHandler *self, tPacket *command)
{
    tPacket *packet;

    DEBUG_PRINT("GetADCInterruptCounter %i\n", adc_interrupt_counter);
    // set timestep
    if (!PacketQueueFull(self->reply_queue)) {
        packet = PacketQueueWrite(self->reply_queue);
        packet->payload[0] = 2; // Reply Code for OK
        packet->size = usnprintf((char *) packet->payload+1, PACKET_QUEUE_PAYLOAD_SIZE-1, "GetADCInterruptCounter %i", adc_interrupt_counter) + 1 + 1; // reply code and string terminator

        PacketQueueWriteDone(self->reply_queue);
    }

}

void
StartADC(tCommandHandler *self, tPacket *command)
{
    DEBUG_PRINT("StartADC\n");
    ADCStart();
}
*/
/*
void
SendLorem(tCommandHandler *self, tPacket *command)
{
    Lorem();
}
*/


typedef void (* const tCommandFunction)(tEvent *);

tCommandFunction commands[] =
{
    NoOperation
};

#define NUM_COMMANDS (sizeof(commands) / sizeof(tCommandFunction))


void
CommandHandlerMain(void)
{
    tEvent *event;
    uint8_t command;

    if (!QueueEmpty(&command_handler.command_queue)) {
        event = QueueRead(&command_handler.command_queue);
        command = event->payload[0];
        if (command < NUM_COMMANDS)
            commands[command](event);
        QueueRelease(&command_handler.command_queue);
    }
}


void
CommandHandlerInit(void)
{
    QueueInit(&command_handler.command_queue);
}

