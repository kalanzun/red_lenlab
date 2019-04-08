/*
 * test_oscilloscope.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "test_oscilloscope.h"

#include "memory.h"
#include "microtest.h"
#include "oscilloscope.h"


void
test_oscilloscope_lock(void)
{
    test();

    assert(oscilloscope.lock == false);
    assert(OscilloscopeStart(&oscilloscope, 1) == OK);
    assert(oscilloscope.lock == true);
    assert(adc_group.lock == true);
    assert(memory.lock == true);

    // wait for the measurement to finish
    while (!OscSeqGroupReady(&oscilloscope.seq_group)) {};

    assert(OscilloscopeStop(&oscilloscope) == OK);
    assert(oscilloscope.lock == false);
    assert(adc_group.lock == false);
    assert(memory.lock == false);

    ok();
}


void
test_oscilloscope_double_start(void)
{
    test();

    assert(OscilloscopeStart(&oscilloscope, 1) == OK);
    assert(OscilloscopeStart(&oscilloscope, 1) == LOCK_ERROR);

    // wait for the measurement to finish
    while (!OscSeqGroupReady(&oscilloscope.seq_group)) {};

    assert(OscilloscopeStop(&oscilloscope) == OK);

    ok();
}

void
test_oscilloscope_double_stop(void)
{
    test();

    assert(OscilloscopeStart(&oscilloscope, 1) == OK);

    // wait for the measurement to finish
    while (!OscSeqGroupReady(&oscilloscope.seq_group)) {};

    assert(OscilloscopeStop(&oscilloscope) == OK);
    assert(OscilloscopeStop(&oscilloscope) == LOCK_ERROR);

    ok();
}


void
test_oscilloscope_adc_error(void)
{
    test();

    adc_group.lock = true;
    assert(OscilloscopeStart(&oscilloscope, 1000) == ADC_ERROR);
    adc_group.lock = false;

    ok();
}


void
test_oscilloscope_memory_error(void)
{
    test();

    memory.lock = true;
    assert(OscilloscopeStart(&oscilloscope, 1000) == MEMORY_ERROR);
    memory.lock = false;

    ok();
}


void
test_oscilloscope_measurement()
{
    unsigned int i, j;
    tRingIter iter;

    tRing *self;
    tPage *page;
    uint16_t *value;

    test();

    for (i = 0; i < MEMORY_LENGTH; i++) {
        for (j = 0; j < PAGE_LENGTH; j++) {
            memory.pages[i].buffer[j] = 0xFFFFFFFF;
        }
    }

    OscilloscopeStart(&oscilloscope, 1);
    while (!OscSeqGroupReady(oscilloscope.seq_group)) {};

    FOREACH_ADC {
        self = &oscilloscope.seq_group->osc_seq[i].ring;
        assert(RingFull(self));

        for (RingIterInit(&iter, self); iter.content; RingIterNext(&iter)) {
            page = RingIterGet(&iter);
            // head
            for (j = 0; j < 6; j++) {
                if (page->buffer[j] != 0xFFFFFFFF)
                    fail("head (adc[%i], page[%i], buffer[%i])", i, iter.read, j);
            }
            // look for alignment error
            // if the alignment is off, uDMA starts at a later address
            if (*(uint8_t *) (page->buffer + 6) == 0xFF)
                fail("alignment (adc[%i], page[%i])", i, iter.read);
            // measurement values
            for (j = 0; j < OSCILLOSCOPE_SAMPLES; j++) {
                value = (uint16_t *) (page->buffer + 6) + j;
                if (*value == 0xFFFF)
                    fail("value (adc[%i], page[%i], buffer[%i])", i, iter.read, j);
            }
        }
    }

    OscilloscopeStop(&oscilloscope);
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
