/*
 * ring.h
 *
 *  Created on: 17.01.2018
 *      Author: christoph
 */

#ifndef RING_H_
#define RING_H_


#include <stdbool.h>
#include <stdint.h>

#include "debug.h"


#define PAGE_LENGTH 1024


typedef struct Page {
    uint8_t buffer[PAGE_LENGTH];
} tPage;


typedef struct Ring {
    tPage *pages;
    uint32_t length;
    volatile uint32_t acquire;
    volatile uint32_t write;
    volatile uint32_t read;
    volatile uint32_t release;
    volatile unsigned char empty;
    volatile unsigned char full;
    volatile unsigned char content;
} tRing;


inline void
RingAllocate(tRing *self, tPage *pages, uint32_t length)
{
    self->pages = pages;
    self->length = length;
    self->acquire = 0;
    self->write = 0;
    self->read = 0;
    self->release = 0;
    self->empty = 1;
    self->content = 0;
}


inline unsigned char
RingEmpty(tRing *self)
{
    return self->empty;
}


inline unsigned char
RingContent(tRing *self)
{
    return self->content;
}


inline unsigned char
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
