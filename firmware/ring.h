/*
 * ring.h
 *
 *  Created on: 17.01.2018
 *      Author: christoph
 */

#ifndef RING_H_
#define RING_H_


#include "memory.h"


typedef struct Ring {
    tPage *pages;
    uint32_t length;
    uint32_t acquire;
    uint32_t write;
    uint32_t read;
    uint32_t release;
    volatile bool empty;
    volatile bool full;
    volatile bool content;
} tRing;


inline void
RingAllocate(tRing *self, uint32_t length)
{
    self->pages = MemoryAllocate(&memory, length);
    self->length = length;
    self->acquire = 0;
    self->write = 0;
    self->read = 0;
    self->release = 0;
    self->empty = 1;
    self->content = 0;
}

inline bool
RingEmpty(tRing *self)
{
    return self->empty;
}

inline bool
RingContent(tRing *self)
{
    return self->content;
}


inline bool
RingFull(tRing *self)
{
    return self->full;
}


inline tPage*
RingAcquire(tRing *self)
{
    tPage *page = self->pages + self->acquire;
    self->acquire = (self->acquire + 1) % self->length;
    self->empty = 0;
    if (self->acquire == self->release) self->full = 1;
    return page;
}


inline void
RingWrite(tRing *self)
{
    self->write = (self->write + 1) % self->length;
    self->content = 1;
}


inline tPage*
RingRead(tRing *self)
{
    tPage *page = self->pages + self->read;
    self->read = (self->read + 1) % self->length;
    if (self->read == self->write) self->content = 0;
    return page;
}


inline void
RingRelease(tRing *self)
{
    self->release = (self->release + 1) % self->length;
    if (self->acquire == self->release) self->empty = 1;
    self->full = 0;
}


inline tPage*
RingGet(tRing *self, uint32_t index)
{
    return self->pages + index;
}


#endif /* RING_H_ */
