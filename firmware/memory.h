/*
 * memory.h
 *
 */

#ifndef MEMORY_H_
#define MEMORY_H_


#include "ring.h"


#define MEMORY_LENGTH 22

typedef struct Memory {

    tPage pages[MEMORY_LENGTH];

    bool lock;

} tMemory;


extern tMemory memory;


inline void
MemoryLock(tMemory *self)
{
    self->lock = 1;
}


inline void
MemoryUnlock(tMemory *self)
{
    self->lock = 0;
}


inline void
MemoryInit(tMemory *self)
{
    self->lock = 0;
}


#endif /* MEMORY_H_ */
