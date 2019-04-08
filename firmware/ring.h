/*
 * ring.h
 *
 */

#ifndef RING_H_
#define RING_H_


#define PAGE_LENGTH 256


typedef struct Page {
    // 4 bytes alignment for uDMA
    uint32_t buffer[PAGE_LENGTH];
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


typedef struct RingIter {
    tRing *ring;
    volatile uint32_t read;
    volatile unsigned char content;
} tRingIter;


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
    self->full = 0;
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
