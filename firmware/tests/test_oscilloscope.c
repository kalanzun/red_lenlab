/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
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

#include "tests/test_oscilloscope.h"

#include "microtest.h"
#include "oscilloscope.h"


#define SAMPLERATE 1


void
test_oscilloscope_lock(void)
{
    test();

    assert(oscilloscope.lock == false);
    assert(OscilloscopeStart(&oscilloscope, SAMPLERATE) == OK);
    assert(oscilloscope.lock == true);
    assert(adc_group.lock == true);
    assert(memory.lock);
    assert(OscilloscopeStop(&oscilloscope) == OK);
    MemoryUnlock(&memory, memory.lock);
    assert(oscilloscope.lock == false);
    assert(adc_group.lock == false);
    assert(memory.lock == 0);

    ok();
}


void
test_oscilloscope_double_start(void)
{
    test();

    assert(OscilloscopeStart(&oscilloscope, SAMPLERATE) == OK);
    assert(OscilloscopeStart(&oscilloscope, SAMPLERATE) == LOCK_ERROR);
    assert(OscilloscopeStop(&oscilloscope) == OK);
    MemoryUnlock(&memory, memory.lock);

    ok();
}

void
test_oscilloscope_double_stop(void)
{
    test();

    assert(OscilloscopeStart(&oscilloscope, SAMPLERATE) == OK);
    assert(OscilloscopeStop(&oscilloscope) == OK);
    MemoryUnlock(&memory, memory.lock);
    assert(OscilloscopeStop(&oscilloscope) == LOCK_ERROR);

    ok();
}


void
test_oscilloscope_adc_error(void)
{
    test();

    adc_group.lock = true;
    assert(OscilloscopeStart(&oscilloscope, SAMPLERATE) == ADC_ERROR);
    adc_group.lock = false;

    ok();
}


void
test_oscilloscope_memory_error(void)
{
    test();

    MemoryLock(&memory, 1);
    assert(OscilloscopeStart(&oscilloscope, SAMPLERATE) == MEMORY_ERROR);
    MemoryUnlock(&memory, memory.lock);

    ok();
}


void
test_oscilloscope_measurement()
{
    unsigned int i, j;
    tRingIter iter;

    tRing *self;
    tPage *page;

    uint8_t *byte_buffer;
    uint16_t *short_buffer;

    test();

    for (i = 0; i < MEMORY_LENGTH; i++) {
        for (j = 0; j < PAGE_LENGTH; j++) {
            memory.pages[i].buffer[j] = 0xFFFFFFFF;
        }
    }

    OscilloscopeStart(&oscilloscope, SAMPLERATE);
    while (oscilloscope.lock) OscilloscopeMain(&oscilloscope, false);
    MemoryUnlock(&memory, memory.lock); // early on, because of return statements

    FOREACH_ADC {
        self = &osc_seq_group.osc_seq[i].ring;
        assert(RingFull(self));

        byte_buffer = (uint8_t *) &i; // the upper bytes are zero

        for (RingIterInit(&iter, self); iter.content; RingIterNext(&iter)) {
            assert(byte_buffer[3] == 0); // previous packet is not last
            page = RingIterGet(&iter);
            // head
            byte_buffer = (uint8_t *) page->buffer;
            if (!(byte_buffer[0] == 5 && byte_buffer[1] == 3 && byte_buffer[2] == i)) {
                fail("head (adc[%i], page[%i], (uint32_t *) buffer[0])", i, iter.read);
                ASSERT(0);
            }
            // watch out for alignment error
            // if the alignment is off, uDMA starts at a later address
            /*// This if does not work, because 0x(!FF)FF is a valid first value
            if (byte_buffer[4] == 0xFF) {
                fail("alignment (adc[%i], page[%i], (uint8_t *) buffer[4]", i, iter.read);
                ASSERT(0);
            }
            */
            // measurement values
            short_buffer = (uint16_t *) page->buffer;
            for (j = OSCILLOSCOPE_OFFSET * 2; j < OSCILLOSCOPE_OFFSET * 2 + OSCILLOSCOPE_SAMPLES; j++) {
                if (short_buffer[j] == 0xFFFF) {
                    fail("value (adc[%i], page[%i], (uint16_t *) buffer[%i])", i, iter.read, j);
                    ASSERT(0);
                }
            }
        }
    }
    assert(byte_buffer[3] == 255); // last packet

    ok();
}

void
test_oscilloscope()
{
    test_oscilloscope_lock();
    test_oscilloscope_double_start();
    test_oscilloscope_double_stop();
    test_oscilloscope_adc_error();
    test_oscilloscope_memory_error();
    test_oscilloscope_measurement();
}
