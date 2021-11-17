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

#ifndef RING_H_
#define RING_H_


#include "driverlib/debug.h"


#define MEMORY_LENGTH 22

#define PAGE_LENGTH 256


typedef struct Page {
    // 4 bytes alignment for uDMA
    uint32_t buffer[PAGE_LENGTH];
} tPage;


typedef struct Memory {
    tPage *pages;
    volatile uint32_t lock; // count of pages, which are locked
} tMemory;


extern tMemory memory;


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


typedef struct RingIter {
    tRing *ring;
    volatile uint32_t read;
    volatile unsigned char content;
} tRingIter;


inline tPage*
MemoryLock(tMemory *self, uint32_t length)
{
    tPage *page;

    ASSERT(self->lock + length <= MEMORY_LENGTH);
    page = self->pages + self->lock;
    self->lock = self->lock + length;
    return page;
}


inline void
MemoryUnlock(tMemory *self, uint32_t length)
{
    ASSERT(length <= self->lock);
    self->lock = self->lock - length;
}


inline void
MemoryInit(tMemory *self, tPage *pages)
{
    self->pages = pages;
    self->lock = 0;
}


inline void
RingAllocate(tRing *self, uint32_t length)
{
    self->pages = MemoryLock(&memory, length);
    self->length = length;
    self->acquire = 0;
    self->write = 0;
    self->read = 0;
    self->release = 0;
    self->empty = 1;
    self->full = 0;
    self->content = 0;
}


inline void
RingFree(tRing *self)
{
    MemoryUnlock(&memory, self->length);
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


inline void
RingIterInit(tRingIter *self, tRing *ring)
{
    self->ring = ring;
    self->read = ring->read;
    self->content = ring->content;
}


inline tPage*
RingIterGet(tRingIter *self)
{
    return self->ring->pages + self->read;
}


inline void
RingIterNext(tRingIter *self)
{
    self->read = (self->read + 1) % self->ring->length;
    if (self->read == self->ring->write) self->content = 0;
}


#endif /* RING_H_ */
