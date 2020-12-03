/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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


#define EVENT_QUEUE_LENGTH 4


typedef struct Event {
    uint8_t payload[LENLAB_PACKET_HEAD_LENGTH + LENLAB_PACKET_BODY_LENGTH];
    uint32_t length;
    tRing *ring;
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
    return (self->write + 1) % EVENT_QUEUE_LENGTH == self->read;
}


inline tEvent*
QueueAcquire(tQueue *self)
{
    ASSERT(!QueueFull(self));
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
    ASSERT(!QueueEmpty(self));
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
    self->payload[1] = noType;
}

inline enum Command
EventGetCommand(tEvent *self)
{
    ASSERT((enum Command) self->payload[0] < NUM_COMMANDS);
    return (enum Command) self->payload[0];
}

inline void
EventSetReply(tEvent *self, enum Reply reply)
{
    self->payload[0] = reply;
    self->payload[1] = noType;
    self->payload[2] = 0;
    self->payload[3] = 255;

    self->ring = 0;
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
    ASSERT((enum Reply) self->payload[0] < NUM_REPLIES);
    return (enum Reply) self->payload[0];
}

inline void
EventSetType(tEvent *self, enum Type type)
{
    self->payload[1] = type;
}

inline enum Type
EventGetType(tEvent *self)
{
    ASSERT((enum Type) self->payload[1] < NUM_TYPES);
    return (enum Type) self->payload[1];
}


inline void
EventSetError(tEvent *self, enum Error error)
{
    self->payload[2] = error;
}

inline enum Error
EventGetError(tEvent *self)
{
    ASSERT((enum Error) self->payload[2] < NUM_ERRORS);
    return (enum Error) self->payload[2];
}

inline void
EventSetLastPackage(tEvent *self, uint8_t last)
{
    self->payload[3] = last;
}

inline uint8_t
EventGetLastPackage(tEvent *self)
{
    return self->payload[3];
}

inline void
EventSetBodyLength(tEvent *self, uint32_t length)
{
    ASSERT(length <= LENLAB_PACKET_BODY_LENGTH);
    self->length = LENLAB_PACKET_HEAD_LENGTH + length;
}

inline uint32_t
EventGetBodyLength(tEvent *self)
{
    return self->length - LENLAB_PACKET_HEAD_LENGTH;
}

inline uint8_t*
EventGetBody(tEvent *self)
{
    return self->payload + LENLAB_PACKET_HEAD_LENGTH;
}

inline void
EventSetString(tEvent *self, const uint8_t *string, uint32_t length)
{
    uint32_t i, len;

    EventSetType(self, String);
    len = length + 1;
    len = len <= LENLAB_PACKET_BODY_LENGTH ? len : LENLAB_PACKET_BODY_LENGTH;
    EventSetBodyLength(self, len);
    for (i = 0; i < (len - 1); i++)
        EventGetBody(self)[i] = string[i];
    EventGetBody(self)[i] = 0;
}

inline void
EventSetIntArray(tEvent *self, const uint32_t array[], uint32_t length)
{
    uint32_t i, len;

    EventSetType(self, IntArray);
    len = length;
    len = len <= LENLAB_PACKET_BODY_LENGTH / 4 ? len : LENLAB_PACKET_BODY_LENGTH / 4;
    EventSetBodyLength(self, 4*len);
    for (i = 0; i < len; i++)
        *(uint32_t *) (EventGetBody(self) + 4*i) = array[i];
}

inline uint8_t
EventGetByte(tEvent *self, uint32_t i)
{
    ASSERT(EventGetType(self) == ByteArray);
    ASSERT(i < EventGetBodyLength(self));
    return EventGetBody(self)[i];
}

inline uint32_t
EventGetInt(tEvent *self, uint32_t i)
{
    ASSERT(EventGetType(self) == IntArray);
    ASSERT(4*i < EventGetBodyLength(self));
    return ((uint32_t *) EventGetBody(self))[i];
}

#endif /* EVENT_QUEUE_H_ */
