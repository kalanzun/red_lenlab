/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>

#include "command_handler.h"

#include "lenlab_version.h"
#include "logger.h"
#include "oscilloscope.h"
#include "reply_handler.h"
#include "signal.h"
#include "trigger.h"


void
on_init(tEvent *event)
{
    tEvent *reply;

    //DEBUG_PRINT("Init");

    LoggerStop(&logger);
    OscilloscopeStop(&oscilloscope);
    TriggerStop(&trigger);
    SignalStop(&signal);

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Init);
    EventSetBodyLength(reply, 0);

    QueueWrite(&reply_handler.reply_queue);
}


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


const uint8_t name[] = "Lenlab red Version " STR(MAJOR) "." STR(MINOR);

#define NAME_LENGTH (sizeof(name)-1)


void
on_getName(tEvent *event)
{
    tEvent *reply;

    //DEBUG_PRINT("getName");

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Name);
    EventSetString(reply, name, NAME_LENGTH);

    QueueWrite(&reply_handler.reply_queue);
}


void
on_getVersion(tEvent *event)
{
    tEvent *reply;
    uint32_t array[2] = {MAJOR, MINOR};

    //DEBUG_PRINT("getVersion");

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Version);
    EventSetIntArray(reply, array, 2);

    QueueWrite(&reply_handler.reply_queue);
}


void
on_startLogger(tEvent *event)
{
    tEvent *reply;
    uint32_t interval = EventGetInt(event, 0);
    tError error;

    //DEBUG_PRINT("startLogger");

    error = LoggerStart(&logger, interval);

    reply = QueueAcquire(&reply_handler.reply_queue);
    EventSetBodyLength(reply, 0);

    if (error) {
        DEBUG_PRINT("Error %i\n", error);
        EventSetReply(reply, Error);
        EventSetError(reply, error);
    }
    else {
        EventSetReply(reply, Logger);
    }

    QueueWrite(&reply_handler.reply_queue);
}


void
on_stopLogger(tEvent *event)
{
    tEvent *reply;
    tError error;

    //DEBUG_PRINT("stopLogger");

    error = LoggerStop(&logger);

    reply = QueueAcquire(&reply_handler.reply_queue);
    EventSetBodyLength(reply, 0);

    if (error) {
        DEBUG_PRINT("Error %i\n", error);
        EventSetReply(reply, Error);
        EventSetError(reply, error);
    }
    else {
        EventSetReply(reply, Logger);
    }

    QueueWrite(&reply_handler.reply_queue);
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

    //DEBUG_PRINT("setSignalSine");

    // this may need a long time
    SignalSetSine(&signal, multiplier, predivider, divider, amplitude, second);

    if (!signal.lock) SignalStart(&signal);

    // send a reply
    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Signal);
    EventSetBodyLength(reply, 0);

    QueueWrite(&reply_handler.reply_queue);
}


void
on_stopSignal(tEvent *event)
{
    tEvent *reply;

    //DEBUG_PRINT("stopSignal");

    if (signal.lock) SignalStop(&signal);

    // send a reply
    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Signal);
    EventSetBodyLength(reply, 0);

    QueueWrite(&reply_handler.reply_queue);
}


void
on_startOscilloscope(tEvent *event)
{
    tEvent *reply;
    uint32_t log2oversamples = EventGetInt(event, 0);
    tError error;

    error = OscilloscopeStart(&oscilloscope, log2oversamples);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        EventSetReply(reply, Error);
        EventSetError(reply, error);
        EventSetBodyLength(reply, 0);
        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_startOscilloscopeLinearTestData(tEvent *event)
{
    tEvent *reply;
    tError error;

    //DEBUG_PRINT("startOscilloscopeLinearTestData");

    error = OscilloscopeLinearTestData(&oscilloscope);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        EventSetReply(reply, Error);
        EventSetError(reply, error);
        EventSetBodyLength(reply, 0);
        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_startTrigger(tEvent *event)
{
    tEvent *reply;
    uint32_t log2oversamples = EventGetInt(event, 0);
    tError error;

    error = TriggerStart(&trigger, log2oversamples);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        EventSetReply(reply, Error);
        EventSetError(reply, error);
        EventSetBodyLength(reply, 0);
        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_startTriggerLinearTestData(tEvent *event)
{
    tEvent *reply;
    tError error;

    //DEBUG_PRINT("startTriggerLinearTestData");

    error = TriggerLinearTestData(&trigger);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        EventSetReply(reply, Error);
        EventSetError(reply, error);
        EventSetBodyLength(reply, 0);
        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_error()
{
    tEvent *reply;

    DEBUG_PRINT("Invalid command");

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Error);
    EventSetBodyLength(reply, 0);

    QueueWrite(&reply_handler.reply_queue);
}


void
CommandHandlerMain(tCommandHandler *self)
{
    tEvent *event;
    enum Command command;

    if (!QueueEmpty(&self->command_queue)) {
        event = QueueRead(&self->command_queue);
        command = EventGetCommand(event);

        if (command == init) on_init(event);
        else if (command == getName) on_getName(event);
        else if (command == getVersion) on_getVersion(event);
        else if (command == startLogger) on_startLogger(event);
        else if (command == stopLogger) on_stopLogger(event);
        else if (command == startOscilloscope) on_startOscilloscope(event);
        else if (command == startTrigger) on_startTrigger(event);
        else if (command == startOscilloscopeLinearTestData) on_startOscilloscopeLinearTestData(event);
        else if (command == startTriggerLinearTestData) on_startTriggerLinearTestData(event);
        else if (command == setSignalSine) on_setSignalSine(event);
        else if (command == stopSignal) on_stopSignal(event);
        else on_error();

        QueueRelease(&self->command_queue);
    }
}

void
CommandHandlerInit(tCommandHandler *self)
{
    QueueInit(&self->command_queue);
}
