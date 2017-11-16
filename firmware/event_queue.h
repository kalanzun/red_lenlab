/*
 * event_queue.h
 *
 *  Created on: 25.10.2017
 *      Author: Christoph
 */

#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_


#include <stdbool.h>
#include <stdint.h>
#include "lenlab_protocol.h"


#define EVENT_QUEUE_LENGTH 8
#define EVENT_PAYLOAD_LENGTH 64


typedef struct Event {
    uint8_t payload[EVENT_PAYLOAD_LENGTH];
    uint32_t length;
} tEvent;


typedef struct Queue {
    tEvent queue[EVENT_QUEUE_LENGTH];
    uint32_t read;
    uint32_t write;
} tQueue;



inline bool
QueueEmpty(tQueue *self)
{
    return self->read == self->write;
}


inline bool
QueueFull(tQueue *self)
{
    return (self->write+1) % EVENT_QUEUE_LENGTH == self->read;
}


inline tEvent*
QueueAcquire(tQueue *self)
{
    return self->queue + self->write;
}


inline void
QueueWrite(tQueue *self)
{
    self->write = (self->write + 1) % EVENT_QUEUE_LENGTH;
}


inline tEvent*
QueueRead(tQueue *self)
{
    return self->queue + self->read;
}


inline void
QueueRelease(tQueue *self)
{
    self->read = (self->read + 1) % EVENT_QUEUE_LENGTH;
}


inline void
QueueInit(tQueue *self)
{
    self->read = 0;
    self->write = 0;
}

inline void
EventSetCommand(tEvent *self, enum Command cmd)
{
    self->payload[0] = cmd;
}

inline enum Command
EventGetCommand(tEvent *self)
{
    return (enum Command) self->payload[0];
}

inline void
EventSetReply(tEvent *self, enum Reply reply)
{
    self->payload[1] = reply;
}

inline enum Reply
EventGetReply(tEvent *self)
{
    return (enum Reply) self->payload[1];
}

#endif /* EVENT_QUEUE_H_ */
