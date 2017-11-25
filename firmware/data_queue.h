/*
 * data_queue.h
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

#ifndef DATA_QUEUE_H_
#define DATA_QUEUE_H_


#include <stdbool.h>
#include <stdint.h>


#define DATA_QUEUE_LENGTH 8
// Caution: USB uDMA sends this size in a single transfer
// It seems to support only values up to 1024 and should be a multiple of the packet size (64)
#define DATA_PAYLOAD_LENGTH 1024


typedef struct DataEvent {
    uint8_t payload[DATA_PAYLOAD_LENGTH];
    uint32_t length;
} tDataEvent;


typedef struct DataQueue {
    tDataEvent queue[DATA_QUEUE_LENGTH];
    uint32_t read;
    uint32_t write;
} tDataQueue;


inline bool
DataQueueEmpty(tDataQueue *self)
{
    return self->read == self->write;
}


inline bool
DataQueueFull(tDataQueue *self)
{
    return (self->write+1) % DATA_QUEUE_LENGTH == self->read;
}


inline tDataEvent*
DataQueueAcquire(tDataQueue *self)
{
    return self->queue + self->write;
}


inline void
DataQueueWrite(tDataQueue *self)
{
    self->write = (self->write + 1) % DATA_QUEUE_LENGTH;
}


inline tDataEvent*
DataQueueRead(tDataQueue *self)
{
    return self->queue + self->read;
}


inline void
DataQueueRelease(tDataQueue *self)
{
    self->read = (self->read + 1) % DATA_QUEUE_LENGTH;
}


inline void
DataQueueInit(tDataQueue *self)
{
    self->read = 0;
    self->write = 0;
}


#endif /* DATA_QUEUE_H_ */
