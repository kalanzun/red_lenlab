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

#include <stdbool.h>
#include <stdint.h>

#include "tests/test_trigger.h"

#include "int_timer.h"
#include "trigger.h"
#include "microtest.h"


// 250 kHz, trigger is too slow for 1 MHz
#define LOG2OVERSAMPLES 2


void
test_trigger_lock()
{
    test();

    assert(trigger.lock == false);
    assert(TriggerStart(&trigger, LOG2OVERSAMPLES) == OK);
    assert(trigger.lock == true);
    assert(adc_group.lock == true);
    assert(memory.lock);
    assert(TriggerStop(&trigger) == OK);
    MemoryUnlock(&memory, memory.lock);
    assert(trigger.lock == false);
    assert(adc_group.lock == false);
    assert(memory.lock == 0);

    ok();
}


void
test_trigger_double_start()
{
    test();

    assert(TriggerStart(&trigger, LOG2OVERSAMPLES) == OK);
    assert(TriggerStart(&trigger, LOG2OVERSAMPLES) == LOCK_ERROR);
    assert(TriggerStop(&trigger) == OK);
    MemoryUnlock(&memory, memory.lock);

    ok();
}


void
test_trigger_double_stop()
{
    test();

    assert(TriggerStart(&trigger, LOG2OVERSAMPLES) == OK);
    assert(TriggerStop(&trigger) == OK);
    MemoryUnlock(&memory, memory.lock);
    assert(TriggerStop(&trigger) == LOCK_ERROR);

    ok();
}


void
test_trigger_adc_error()
{
    test();

    adc_group.lock = true;
    assert(TriggerStart(&trigger, LOG2OVERSAMPLES) == ADC_ERROR);
    adc_group.lock = false;

    ok();
}


void
test_trigger_memory_error(void)
{
    test();

    MemoryLock(&memory, 1);
    assert(TriggerStart(&trigger, LOG2OVERSAMPLES) == MEMORY_ERROR);
    MemoryUnlock(&memory, memory.lock);

    ok();
}


void
test_trigger_measurement()
{
    unsigned int i, j;

    tRingIter iter;
    tPage *page;

    uint8_t *head;
    int8_t *buffer;

    test();

    for (i = 0; i < MEMORY_LENGTH; i++) {
        memory.pages[i].buffer[0] = 0xFFFFFFFF;
        buffer = (int8_t *) memory.pages[i].buffer;
        for (j = 4; j < 4 * PAGE_LENGTH; j++) {
            buffer[j] = -128;
        }
    }

    TriggerStart(&trigger, LOG2OVERSAMPLES);
    while (trigger.lock) TriggerMain(&trigger, false);
    MemoryUnlock(&memory, memory.lock); // early on, because of return statements

    i = 0;
    head = (uint8_t *) &i; // point head to 4 bytes of zero

    for (RingIterInit(&iter, &trigger.ring); iter.content; RingIterNext(&iter)) {
        assert(head[3] == 0); // previous packet is not last
        page = RingIterGet(&iter);
        // head
        head = (uint8_t *) (page->buffer);
        buffer = (int8_t *) (page->buffer);
        if (!(head[0] == 5 && head[1] == 2 && head[2] == 0)) {
            fail("head (page[%i], (uint32_t *) buffer[0])", iter.read);
            ASSERT(0);
        }
        for (j = 4; j < 12; j++) {
            if (buffer[j] == -128) {
                fail("head (page[%i], (int8_t *) buffer[%i])", iter.read, j);
                ASSERT(0);
            }
        }
        // measurement values
        for (j = OSCILLOSCOPE_OFFSET * 4 + 2; j < OSCILLOSCOPE_OFFSET * 4 + OSCILLOSCOPE_SAMPLES * 2; j++) {
            if (buffer[j] == -128) {
                fail("value (page[%i], (uint8_t *) buffer[%i])", iter.read, j);
                ASSERT(0);
            }
        }
    }
    assert(head[3] == 255); // last packet

    ok();
}


void
test_trigger()
{
    test_trigger_lock();
    test_trigger_double_start();
    test_trigger_double_stop();
    test_trigger_adc_error();
    test_trigger_memory_error();
    test_trigger_measurement();
}
