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

#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_


#include "driverlib/debug.h"

#include "debug.h"
#include "error.h"
#include "ring.h"
#include "lenlab_protocol.h"


#define EVENT_QUEUE_LENGTH 8


typedef union Event {
    union {
        uint8_t payload[LENLAB_PACKET_HEAD_LENGTH + LENLAB_PACKET_BODY_LENGTH];
        struct {
            uint8_t code;
            uint8_t type;
            uint8_t error;
            uint8_t last;
            union {
                uint8_t body[LENLAB_PACKET_BODY_LENGTH];
                uint32_t array[LENLAB_PACKET_BODY_LENGTH / 4];
            };
        };
    };
    tRing *ring;
} tEvent;


#define EVENT_QUEUE_BUFFER_LENGTH (EVENT_QUEUE_LENGTH * sizeof(tEvent))


typedef struct Meta {
    uint8_t length;
} tMeta;


typedef struct Queue {
    tEvent *queue;
    tMeta meta[EVENT_QUEUE_LENGTH];

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
    return (self->write + 1) % EVENT_QUEUE_LENGTH == self->read;
}


inline uint32_t
QueueElementCount(tQueue *self)
{
    return ((int32_t) self->write - (int32_t) self->read) % EVENT_QUEUE_LENGTH;
}


inline tEvent*
QueueAcquire(tQueue *self)
{
    ASSERT(!QueueFull(self));
    return self->queue + self->write;
}


inline void
QueueSetEventLength(tQueue *self, uint8_t length)
{
    self->meta[self->write].length = length;
}


inline void
QueueSetEventBodyLength(tQueue *self, uint8_t length)
{
    QueueSetEventLength(self, length + LENLAB_PACKET_HEAD_LENGTH);
}


inline void
QueueWrite(tQueue *self)
{
    self->write = (self->write + 1) % EVENT_QUEUE_LENGTH;
}


inline tEvent*
QueueRead(tQueue *self)
{
    ASSERT(!QueueEmpty(self));
    return self->queue + self->read;
}


inline uint8_t
QueueGetEventLength(tQueue *self)
{
    return self->meta[self->read].length;
}


inline uint8_t
QueueGetEventBodyLength(tQueue *self)
{
    return QueueGetEventLength(self) - LENLAB_PACKET_HEAD_LENGTH;
}


inline void
QueueRelease(tQueue *self)
{
    self->read = (self->read + 1) % EVENT_QUEUE_LENGTH;
}


inline void
QueueInit(tQueue *self, uint8_t *buffer)
{
    self->queue = (tEvent *) buffer;
    self->read = 0;
    self->write = 0;
}


inline void
EventSetCommand(tEvent *self, enum Command cmd)
{
    self->code = cmd;
    self->type = noType;
}


inline enum Command
EventGetCommand(tEvent *self)
{
    ASSERT((enum Command) self->code < NUM_COMMANDS);
    return (enum Command) self->code;
}


inline void
EventSetReply(tEvent *self, enum Reply reply)
{
    self->code = reply;
    self->type = noType;
    self->error = 0;
    self->last = 255;
}


inline void
EventSetRing(tEvent *self, tRing *ring)
{
    self->ring = ring;
}


inline tRing*
EventGetRing(tEvent *self)
{
    return self->ring;
}


inline enum Reply
EventGetReply(tEvent *self)
{
    ASSERT((enum Reply) self->code < NUM_REPLIES);
    return (enum Reply) self->code;
}


inline void
EventSetType(tEvent *self, enum Type type)
{
    self->type = type;
}


inline enum Type
EventGetType(tEvent *self)
{
    ASSERT((enum Type) self->type < NUM_TYPES);
    return (enum Type) self->type;
}


inline void
EventSetError(tEvent *self, enum Error error)
{
    self->error = error;
}


inline enum Error
EventGetError(tEvent *self)
{
    ASSERT((enum Error) self->error < NUM_ERRORS);
    return (enum Error) self->error;
}


inline void
EventSetLastPackage(tEvent *self, uint8_t last)
{
    self->last = last;
}


inline uint8_t
EventGetLastPackage(tEvent *self)
{
    return self->last;
}


inline uint8_t*
EventGetBody(tEvent *self)
{
    return self->body;
}


inline void
QueueSetString(tQueue *self, const uint8_t *string, uint32_t length)
{
    tEvent *event = QueueAcquire(self);
    uint32_t i, len;

    event->type = String;
    len = length + 1;
    len = len <= LENLAB_PACKET_BODY_LENGTH ? len : LENLAB_PACKET_BODY_LENGTH;
    QueueSetEventBodyLength(self, len);
    for (i = 0; i < (len - 1); i++)
        event->body[i] = string[i];
    event->body[i] = 0;
}


inline void
QueueSetIntArray(tQueue *self, const uint32_t array[], uint32_t length)
{
    tEvent *event = QueueAcquire(self);
    uint32_t i, len;

    event->type = IntArray;
    len = length;
    len = len <= LENLAB_PACKET_BODY_LENGTH / 4 ? len : LENLAB_PACKET_BODY_LENGTH / 4;
    QueueSetEventBodyLength(self, 4*len);
    for (i = 0; i < len; i++)
        event->array[i] = array[i];
}


inline uint8_t
QueueGetByte(tQueue *self, uint32_t i)
{
    tEvent *event = QueueRead(self);

    ASSERT(event->type == ByteArray);
    ASSERT(i < QueueGetEventBodyLength(self));
    return event->body[i];
}


inline uint32_t
QueueGetInt(tQueue *self, uint32_t i)
{
    tEvent *event = QueueRead(self);

    ASSERT(event->type == IntArray);
    ASSERT(4*i < QueueGetEventBodyLength(self));
    return event->array[i];
}


#endif /* EVENT_QUEUE_H_ */
