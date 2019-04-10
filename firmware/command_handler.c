/*
 * command_handler.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "command_handler.h"

#include "lenlab_version.h"
#include "logger.h"
#include "oscilloscope.h"
#include "reply_handler.h"
#include "trigger.h"


void
on_init(tEvent *event)
{
    tEvent *reply;

    DEBUG_PRINT("Init\n");

    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, Init);
    EventSetBodyLength(reply, 0);

    QueueWrite(&reply_handler.reply_queue);
}


const uint8_t name[] = "Lenlab red Firmware Version " STR(MAJOR) "." STR(MINOR);

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
    uint32_t array[2] = {MAJOR, MINOR};

    DEBUG_PRINT("getVersion\n");

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

    DEBUG_PRINT("startLogger\n");

    error = LoggerStart(&logger, interval);

    reply = QueueAcquire(&reply_handler.reply_queue);
    EventSetBodyLength(reply, 0);

    if (error) {
        DEBUG_PRINT("Error %i\n", error);
        EventSetReply(reply, Error);
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

    DEBUG_PRINT("stopLogger\n");

    error = LoggerStop(&logger);

    reply = QueueAcquire(&reply_handler.reply_queue);
    EventSetBodyLength(reply, 0);

    if (error) {
        DEBUG_PRINT("Error %i\n", error);
        EventSetReply(reply, Error);
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

    DEBUG_PRINT("setSignalSine\n");

    // this may need a long time
    //SignalSetSine(multiplier, predivider, divider, amplitude, second);

    // send a reply
    reply = QueueAcquire(&reply_handler.reply_queue);

    EventSetReply(reply, SignalSine);
    EventSetBodyLength(reply, 0);

    QueueWrite(&reply_handler.reply_queue);
}


void
on_stopSignal(tEvent *event)
{
    DEBUG_PRINT("stopSignal\n");

    //SignalStop();
}


void
on_startOscilloscope(tEvent *event)
{
    tEvent *reply;
    uint32_t samplerate = EventGetInt(event, 0);
    tError error;

    DEBUG_PRINT("startOscilloscope\n");

    error = OscilloscopeStart(&oscilloscope, samplerate);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        EventSetReply(reply, Error);
        EventSetBodyLength(reply, 0);
        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_startOscilloscopeTrigger(tEvent *event)
{
    tEvent *reply;
    uint32_t samplerate = EventGetInt(event, 0);
    uint32_t error;

    DEBUG_PRINT("startOscilloscopeTrigger\n");

    error = TriggerStart(&trigger, samplerate);

    if (error) {
        reply = QueueAcquire(&reply_handler.reply_queue);
        EventSetReply(reply, Error);
        EventSetBodyLength(reply, 0);
        QueueWrite(&reply_handler.reply_queue);
    }
}


void
on_error()
{
    tEvent *reply;

    DEBUG_PRINT("Invalid command\n");

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
        else if (command == setSignalSine) on_setSignalSine(event);
        else if (command == stopSignal) on_stopSignal(event);
        else if (command == startOscilloscope) on_startOscilloscope(event);
        else if (command == startOscilloscopeTrigger) on_startOscilloscopeTrigger(event);
        else if (command == stopLogger) on_stopLogger(event);
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
