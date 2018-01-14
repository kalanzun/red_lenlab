/*
 * command_handler.c
 *

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <stdbool.h>
#include <stdint.h>
#include "utils/ustdlib.h"
#include "driverlib/debug.h"
#include "debug.h"
#include "command_handler.h"
#include "usb_device.h"
#include "adc.h"
#include "reply_handler.h"
#include "timer.h"
#include "logger.h"
#include "config.h"
#include "oscilloscope.h"
#include "signal.h"


tCommandHandler command_handler;


void
on_init(tEvent *event)
{
    tEvent *reply;

    DEBUG_PRINT("init\n");

    reply = QueueAcquire(&reply_handler.reply_queue);

    //LoggerStop();

    EventSetReply(reply, Init);
    EventSetBodyLength(reply, 0);

    QueueWrite(&reply_handler.reply_queue);
}

const uint8_t name[] = "Lenlab red Firmware Version " STR(MAJOR) "." STR(MINOR) "." STR(REVISION);

#define NAME_LENGTH (sizeof(name)-1)

void
on_getName(tEvent *event)
{
    tEvent *reply;

    DEBUG_PRINT("getName\n");

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Name);
    EventSetString(reply, name, NAME_LENGTH);

    QueueWrite(&reply_handler.reply_queue);
}

void
on_getVersion(tEvent *event)
{
    tEvent *reply;
    uint32_t array[3] = {MAJOR, MINOR, REVISION};

    DEBUG_PRINT("getVersion\n");

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Version);
    EventSetIntArray(reply, array, 3);

    QueueWrite(&reply_handler.reply_queue);
}

/*
void
on_setLoggerInterval(tEvent *event)
{
    uint32_t value = *(uint32_t *) (event->payload + 4);

    DEBUG_PRINT("setLoggerInterval\n");

    LoggerSetInterval(value);
}

void
on_startLogger(tEvent *event)
{
    DEBUG_PRINT("startLogger\n");

    LoggerStart();
}

void
on_stopLogger(tEvent *event)
{
    DEBUG_PRINT("stopLogger\n");

    LoggerStop();
}

void
on_calculateSine(tEvent *event)
{
    DEBUG_PRINT("on_calculateSine\n");
    SignalCalculateSine();
}
*/
void
on_startOscilloscope(tEvent *event)
{
    //DEBUG_PRINT("startOscilloscope\n");

    OscilloscopeStart(&oscilloscope);
}

void
on_setSignalSine(tEvent *event)
{
    uint8_t multiplier = EventGetByte(event, 0);
    uint8_t predivider = EventGetByte(event, 1);
    uint8_t divider    = EventGetByte(event, 2);

    SignalSetSine(multiplier, predivider, divider);
}

/*
void
on_testSignalgeneratorSineFrequency(tEvent *event)
{
    SignalTestSineFrequency(event);
}
*/

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

/*
typedef void (* const tCommandFunction)(tEvent *);

tCommandFunction commands[] =
{
    NoOperation,
    getName,
    acquireLogger,
    releaseLogger,
    setLoggerInterval,
    startLogger,
    stopLogger
};

#define NUM_COMMANDS (sizeof(commands) / sizeof(tCommandFunction))
*/

void
CommandHandlerMain(void)
{
    tEvent *event;
    enum Command command;

    if (!QueueEmpty(&command_handler.command_queue)) {
        event = QueueRead(&command_handler.command_queue);
        command = EventGetCommand(event);
        if (command == init) on_init(event);
        else if (command == getName) on_getName(event);
        else if (command == getVersion) on_getVersion(event);
        else if (command == startOscilloscope) on_startOscilloscope(event);
        else if (command == setSignalSine) on_setSignalSine(event);
        /*
        else if (command == setLoggerInterval) on_setLoggerInterval(event);
        else if (command == startLogger) on_startLogger(event);
        else if (command == stopLogger) on_stopLogger(event);
        else if (command == calculateSine) on_calculateSine(event);
        else if (command == testSignalgeneratorSineFrequency) on_testSignalgeneratorSineFrequency(event);
        */
        QueueRelease(&command_handler.command_queue);
    }
}


void
CommandHandlerInit(void)
{
    QueueInit(&command_handler.command_queue);
}

