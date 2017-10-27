/*
 * data_queue.h
 *
 *  Created on: 27.10.2017
 *      Author: Christoph
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
