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


struct Ring {
    uint8_t *const array;
    const uint32_t length;
    const uint32_t element_size;

    volatile uint32_t read;
    volatile uint32_t write;

    volatile bool has_content;
    volatile bool has_space;
};


#define RING_NEW(_array) { \
    .array = (uint8_t *) (_array), \
    .length = sizeof(_array) / sizeof((_array)[0]), \
    .element_size = sizeof((_array)[0]), \
    .has_space = true \
}


inline uint8_t *
RingAcquire(struct Ring *self)
{
    ASSERT(self->has_space);

    return self->array + self->write * self->element_size;
}


inline void
RingWrite(struct Ring *self)
{
    ASSERT(self->has_space);

    self->write = (self->write + 1) % self->length;
    self->has_content = true;
    if (self->read == self->write) self->has_space = false;
}


inline uint8_t *
RingRead(struct Ring *self)
{
    ASSERT(self->has_content);

    return self->array + self->read * self->element_size;
}


inline void
RingRelease(struct Ring *self)
{
    ASSERT(self->has_content);

    self->read = (self->read + 1) % self->length;
    if (self->read == self->write) self->has_content = false;
    self->has_space = true;
}


#endif /* RING_H_ */
