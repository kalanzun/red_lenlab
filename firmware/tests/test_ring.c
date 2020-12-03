/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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

#include <stdbool.h>
#include <stdint.h>

#include "tests/test_ring.h"

#include "microtest.h"
#include "oscilloscope.h"


void
test_ring_allocate()
{
    tRing ring;
    test();

    assert(memory.lock == 0);
    RingAllocate(&ring, MEMORY_LENGTH);
    assert(memory.lock == MEMORY_LENGTH);

    RingFree(&ring);
    assert(memory.lock == 0);

    ok();
}


void
test_ring_empty()
{
    tRing ring;
    test();

    RingAllocate(&ring, MEMORY_LENGTH);

    assert(RingEmpty(&ring));
    assert(!RingContent(&ring));
    assert(!RingFull(&ring));

    RingFree(&ring);

    ok();
}


void
test_ring_content()
{
    tRing ring;
    test();

    RingAllocate(&ring, MEMORY_LENGTH);
    RingAcquire(&ring);
    assert(!RingEmpty(&ring));
    assert(!RingContent(&ring));

    RingWrite(&ring);
    assert(!RingEmpty(&ring));
    assert(RingContent(&ring));

    assert(!RingFull(&ring));

    RingFree(&ring);

    ok();
}


void
test_ring_full()
{
    int i;
    tRing ring;
    test();

    RingAllocate(&ring, MEMORY_LENGTH);
    for (i = 0; i < MEMORY_LENGTH; i++) {
        RingAcquire(&ring);
        RingWrite(&ring);
    }

    assert(!RingEmpty(&ring));
    assert(RingContent(&ring));
    assert(RingFull(&ring));

    RingFree(&ring);

    ok();
}


void
test_ring_read()
{
    tRing ring;
    test();

    RingAllocate(&ring, MEMORY_LENGTH);
    RingAcquire(&ring);
    RingWrite(&ring);

    RingRead(&ring);
    assert(!RingContent(&ring));
    assert(!RingEmpty(&ring));

    RingRelease(&ring);
    assert(!RingContent(&ring));
    assert(RingEmpty(&ring));

    assert(!RingFull(&ring));

    RingFree(&ring);

    ok();
}


void
test_ring_iter()
{
    int i;
    tRing ring;
    tRingIter iter;
    test();

    RingAllocate(&ring, MEMORY_LENGTH);
    RingAcquire(&ring);
    RingWrite(&ring);

    i = 0;
    for (RingIterInit(&iter, &ring); iter.content; RingIterNext(&iter)) {
        i += 1;
    }

    assert(i == 1);
    assert(RingContent(&ring));
    assert(!RingEmpty(&ring));
    assert(!RingFull(&ring));

    RingFree(&ring);

    ok();
}


void
test_ring_fill_up()
{
    int i;
    tRing ring;
    test();

    RingAllocate(&ring, MEMORY_LENGTH);
    for (i = 0; !RingFull(&ring); i++) {
        RingAcquire(&ring);
        RingWrite(&ring);
    }

    assert(i == MEMORY_LENGTH);

    for (i = 0; !RingEmpty(&ring); i++) {
        RingRead(&ring);
        RingRelease(&ring);
    }

    assert(i == MEMORY_LENGTH);

    RingFree(&ring);

    ok();
}


void
test_ring_wrap_fill_up()
{
    int i;
    tRing ring;
    test();

    RingAllocate(&ring, MEMORY_LENGTH);

    for (i = 0; i < MEMORY_LENGTH / 2; i++) {
        RingAcquire(&ring);
        RingWrite(&ring);
        RingRead(&ring);
        RingRelease(&ring);
    }

    for (i = 0; !RingFull(&ring); i++) {
        RingAcquire(&ring);
        RingWrite(&ring);
    }

    assert(i == MEMORY_LENGTH);

    for (i = 0; !RingEmpty(&ring); i++) {
        RingRead(&ring);
        RingRelease(&ring);
    }

    assert(i == MEMORY_LENGTH);

    RingFree(&ring);

    ok();
}


void
test_ring()
{
    test_ring_allocate();
    test_ring_empty();
    test_ring_content();
    test_ring_full();
    test_ring_read();
    test_ring_iter();
    test_ring_fill_up();
    test_ring_wrap_fill_up();
}
