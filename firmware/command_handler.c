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
on_init(tCommandHandler *self)
{
    tEvent *reply;

    LoggerStop(&logger);
    OscilloscopeStop(&oscilloscope);
    TriggerStop(&trigger);
    SignalStop(&signal);

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Init);
    QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

    QueueWrite(&reply_handler.reply_queue);
}


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


const uint8_t name[] = "Lenlab red Firmware Version " STR(MAJOR) "." STR(MINOR);

#define NAME_LENGTH (sizeof(name)-1)


void
on_getName(tCommandHandler *self)
{
    tEvent *reply;

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Name);
    QueueSetString(&reply_handler.reply_queue, name, NAME_LENGTH);

    QueueWrite(&reply_handler.reply_queue);
}


void
on_getVersion(tCommandHandler *self)
{
    tEvent *reply;
    uint32_t array[2] = {MAJOR, MINOR};

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Version);
    QueueSetIntArray(&reply_handler.reply_queue, array, 2);

    QueueWrite(&reply_handler.reply_queue);
}


void
on_startLogger(tCommandHandler *self)
{
    tEvent *reply;
    uint32_t interval = QueueGetInt(&self->command_queue, 0);
    tError error;

    error = LoggerStart(&logger, interval);

    reply = QueueAcquire(&reply_handler.reply_queue);
    QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

    if (error) {
        EventSetReply(reply, Error);
        EventSetError(reply, error);
        DEBUG_PRINT("startLogger error %i", error);
    }
    else {
        EventSetReply(reply, Logger);
    }

    QueueWrite(&reply_handler.reply_queue);
}


void
on_stopLogger(tCommandHandler *self)
{
    tEvent *reply;
    tError error;

    error = LoggerStop(&logger);

    reply = QueueAcquire(&reply_handler.reply_queue);
    QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

    if (error) {
        EventSetReply(reply, Error);
        EventSetError(reply, error);
        DEBUG_PRINT("stopLogger error %i", error);
    }
    else {
        EventSetReply(reply, Logger);
    }

    QueueWrite(&reply_handler.reply_queue);
}


void
on_setSignalSine(tCommandHandler *self)
{
    tEvent *reply;

    uint32_t multiplier = QueueGetInt(&self->command_queue, 0);
    uint32_t predivider = QueueGetInt(&self->command_queue, 1);
    uint32_t divider    = QueueGetInt(&self->command_queue, 2);
    uint32_t amplitude  = QueueGetInt(&self->command_queue, 3);
    uint32_t second     = QueueGetInt(&self->command_queue, 4);

    // this may need a long time
    SignalSetSine(&signal, multiplier, predivider, divider, amplitude, second);

    if (!signal.lock) SignalStart(&signal);

    // send a reply
    reply = QueueAcquire(&reply_handler.reply_queue);
    QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

    EventSetReply(reply, Signal);

    QueueWrite(&reply_handler.reply_queue);
}


void
on_stopSignal(tCommandHandler *self)
{
    tEvent *reply;

    if (signal.lock) SignalStop(&signal);

    // send a reply
    reply = QueueAcquire(&reply_handler.reply_queue);
    QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

    EventSetReply(reply, Signal);

    QueueWrite(&reply_handler.reply_queue);
}


void
on_startOscilloscope(tCommandHandler *self)
{
    tEvent *reply;
    uint32_t log2oversamples = QueueGetInt(&self->command_queue, 0);
    tError error;

    error = OscilloscopeStart(&oscilloscope, log2oversamples);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

        EventSetReply(reply, Error);
        EventSetError(reply, error);
        DEBUG_PRINT("startOscilloscope error %i", error);

        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_startOscilloscopeLinearTestData(tCommandHandler *self)
{
    tEvent *reply;
    tError error;

    error = OscilloscopeLinearTestData(&oscilloscope);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

        EventSetReply(reply, Error);
        EventSetError(reply, error);
        DEBUG_PRINT("startOscilloscopeLinearTestData error %i", error);

        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_startTrigger(tCommandHandler *self)
{
    tEvent *reply;
    uint32_t log2oversamples = QueueGetInt(&self->command_queue, 0);
    tError error;

    error = TriggerStart(&trigger, log2oversamples);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

        EventSetReply(reply, Error);
        EventSetError(reply, error);
        DEBUG_PRINT("startTrigger error %i", error);

        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_startTriggerLinearTestData(tCommandHandler *self)
{
    tEvent *reply;
    tError error;

    error = TriggerLinearTestData(&trigger);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

        EventSetReply(reply, Error);
        EventSetError(reply, error);
        DEBUG_PRINT("startTriggerLinearTestData error %i", error);

        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_error()
{
    tEvent *reply;

    DEBUG_PRINT("Invalid command");

    reply = QueueAcquire(&reply_handler.reply_queue);
    QueueSetEventBodyLength(&reply_handler.reply_queue, 0);

    EventSetReply(reply, Error);

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

        if (command == init) on_init(self);
        else if (command == getName) on_getName(self);
        else if (command == getVersion) on_getVersion(self);
        else if (command == startLogger) on_startLogger(self);
        else if (command == stopLogger) on_stopLogger(self);
        else if (command == startOscilloscope) on_startOscilloscope(self);
        else if (command == startTrigger) on_startTrigger(self);
        else if (command == startOscilloscopeLinearTestData) on_startOscilloscopeLinearTestData(self);
        else if (command == startTriggerLinearTestData) on_startTriggerLinearTestData(self);
        else if (command == setSignalSine) on_setSignalSine(self);
        else if (command == stopSignal) on_stopSignal(self);
        else on_error();

        QueueRelease(&self->command_queue);
    }
}

void
CommandHandlerInit(tCommandHandler *self, uint8_t *command_buffer)
{
    QueueInit(&self->command_queue, command_buffer);
}
