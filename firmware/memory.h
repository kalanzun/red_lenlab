/*
 * memory.h
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

#ifndef MEMORY_H_
#define MEMORY_H_


#define MEMORY_LENGTH 16
#define MEMORY_PAGE_LENGTH 1024


typedef struct Page {
    uint8_t buffer[MEMORY_PAGE_LENGTH];
} tPage;


typedef struct Memory {
    tPage pages[MEMORY_LENGTH];
    uint32_t read;
    uint32_t write;
    bool send;
} tMemory;


extern tMemory memory;


inline void
MemoryAllocate(tMemory *self)
{
    self->read = 0;
    self->write = 0;
    self->send = false;
}


inline void
MemoryFree(tMemory *self)
{

}


inline bool
MemorySend(tMemory *self)
{
    return self->send;
}


inline bool
MemoryEmpty(tMemory *self)
{
    return self->read == self->write;
}


inline bool
MemoryFull(tMemory *self)
{
    return (self->write+1) % MEMORY_LENGTH == self->read;
}


inline tPage*
MemoryAcquire(tMemory *self)
{
    return self->pages + self->write;
}


inline void
MemoryWrite(tMemory *self)
{
    self->write = (self->write + 1) % MEMORY_LENGTH;
}


inline void
MemoryStartSending(tMemory *self, uint32_t start)
{
    self->read = start;
    self->send = true;
}


inline tPage*
MemoryRead(tMemory *self)
{
    return self->pages + self->read;
}


inline void
MemoryRelease(tMemory *self)
{
    self->read = (self->read + 1) % MEMORY_LENGTH;
    if (MemoryEmpty(self)) self->send = false;
}


inline void
MemoryInit(tMemory *self)
{
}


#endif /* MEMORY_H_ */
