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

    //DEBUG_PRINT("getName\n");

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Name);
    EventSetString(reply, name, NAME_LENGTH);
    EventSetLastPackage(reply);

    QueueWrite(&reply_handler.reply_queue);
}

void
on_getVersion(tEvent *event)
{
    tEvent *reply;
    uint32_t array[3] = {MAJOR, MINOR, REVISION};

    //DEBUG_PRINT("getVersion\n");

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Version);
    EventSetIntArray(reply, array, 3);
    EventSetLastPackage(reply);

    QueueWrite(&reply_handler.reply_queue);
}

void
on_startOscilloscope(tEvent *event)
{
    uint32_t samplerate = EventGetInt(event, 0);

    //DEBUG_PRINT("startOscilloscope\n");

    OscilloscopeStart(&oscilloscope, samplerate);
}

void
on_setSignalSine(tEvent *event)
{
    tEvent *reply;

    uint32_t multiplier = EventGetInt(event, 0);
    uint32_t predivider = EventGetInt(event, 1);
    uint32_t divider    = EventGetInt(event, 2);
    uint32_t amplitude  = EventGetInt(event, 3);
    uint32_t second     = EventGetInt(event, 4);

    //DEBUG_PRINT("setSignalSine\n");

    // this may need a long time
    SignalSetSine(multiplier, predivider, divider, amplitude, second);

    // send a reply
    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, SignalSine);
    EventSetBodyLength(reply, 0);

    QueueWrite(&reply_handler.reply_queue);
}

void
on_stopSignal(tEvent *event)
{
    //DEBUG_PRINT("stopSignal\n");

    SignalStop();
}

void
on_startOscilloscopeTrigger(tEvent *event)
{
    uint32_t samplerate = EventGetInt(event, 0);

    //DEBUG_PRINT("startOscilloscopeTrigger\n");

    OscilloscopeStartTrigger(&oscilloscope, samplerate);
}

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
        else if (command == setSignalSine) on_setSignalSine(event);
        else if (command == stopSignal) on_stopSignal(event);
        else if (command == startOscilloscope) on_startOscilloscope(event);
        else if (command == startOscilloscopeTrigger) on_startOscilloscopeTrigger(event);

        QueueRelease(&command_handler.command_queue);
    }
}

void
CommandHandlerInit(void)
{
    QueueInit(&command_handler.command_queue);
}
