/*
 * test_trigger.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "tests/test_trigger.h"

#include "int_timer.h"
#include "trigger.h"
#include "microtest.h"


void
test_trigger_lock()
{
    test();

    assert(trigger.lock == false);
    assert(TriggerStart(&trigger, 1) == OK);
    assert(trigger.lock == true);
    assert(adc_group.lock == true);
    assert(memory.lock == true);
    assert(TriggerStop(&trigger) == OK);
    assert(trigger.lock == false);
    assert(adc_group.lock == false);
    assert(memory.lock == false);

    ok();
}


void
test_trigger_double_start()
{
    test();

    assert(TriggerStart(&trigger, 1) == OK);
    assert(TriggerStart(&trigger, 1) == LOCK_ERROR);
    assert(TriggerStop(&trigger) == OK);

    ok();
}


void
test_trigger_double_stop()
{
    test();

    assert(TriggerStart(&trigger, 1) == OK);
    assert(TriggerStop(&trigger) == OK);
    assert(TriggerStop(&trigger) == LOCK_ERROR);

    ok();
}


void
test_trigger_adc_error()
{
    test();

    adc_group.lock = true;
    assert(TriggerStart(&trigger, 1) == ADC_ERROR);
    adc_group.lock = false;

    ok();
}


void
test_trigger_measurement()
{
    unsigned int i, j;

    tRingIter iter;
    tPage *page;

    int8_t *buffer;

    test();

    for (i = 0; i < MEMORY_LENGTH; i++) {
        memory.pages[i].buffer[0] = 0xFFFFFFFF;
        buffer = (int8_t *) memory.pages[i].buffer;
        for (j = 4; j < 4 * PAGE_LENGTH; j++) {
            buffer[j] = -128;
        }
    }

    // 500 kHz, trigger is too slow for 1 MHz
    TriggerStart(&trigger, 2);
    while (trigger.lock) TriggerMain(&trigger);

    for (RingIterInit(&iter, &trigger.ring); iter.content; RingIterNext(&iter)) {
        page = RingIterGet(&iter);
        // head
        if (!(page->buffer[0] == 0xFFFFFFFF)) {
            fail("head (page[%i], (uint32_t *) buffer[0])", iter.read);
            return;
        }
        // alignment
        buffer = (int8_t *) (page->buffer);
        for (j = 4; j < 6; j++) {
            if (buffer[j] != -128) {
                fail("alignment (page[%i], (uint8_t *) buffer[%i])", iter.read, j);
                return;
            }
        }
        // measurement values
        for (j = 0; j < 2 * OSCILLOSCOPE_SAMPLES; j++) {
            if (buffer[j + 6] == -128) {
                fail("value (page[%i], (uint8_t *) buffer[%i])", iter.read, j + 6);
                return;
            }
        }
    }

    ok();
}


void
test_trigger()
{
    test_trigger_lock();
    test_trigger_double_start();
    test_trigger_double_stop();
    test_trigger_adc_error();
    test_trigger_measurement();
}
