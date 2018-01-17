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
#define PAGE_LENGTH 1024


typedef struct Page {
    uint8_t buffer[PAGE_LENGTH];
} tPage;


typedef struct Memory {
    tPage memory[MEMORY_LENGTH];
    uint32_t allocated;
} tMemory;


extern tMemory memory;


inline tPage*
MemoryAllocate(tMemory *self, uint32_t pages)
{
    tPage *page;
    ASSERT(self->allocated + pages <= MEMORY_LENGTH);
    page = self->memory + self->allocated;
    self->allocated = self->allocated + pages;
    return page;
}


inline void
MemoryInit(tMemory *self)
{
    self->allocated = 0;
}


#endif /* MEMORY_H_ */
