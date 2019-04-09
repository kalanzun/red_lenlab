/*
 * test_oscilloscope.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "test_oscilloscope.h"

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
    assert(memory.acquire);

    // wait for the measurement to finish
    while (!OscSeqGroupReady(oscilloscope.seq_group)) {};

    assert(OscilloscopeStop(&oscilloscope) == OK);
    assert(oscilloscope.lock == false);
    assert(adc_group.lock == false);
    assert(memory.acquire == 0);

    ok();
}


void
test_oscilloscope_double_start(void)
{
    test();

    assert(OscilloscopeStart(&oscilloscope, 1) == OK);
    assert(OscilloscopeStart(&oscilloscope, 1) == LOCK_ERROR);

    // wait for the measurement to finish
    while (!OscSeqGroupReady(oscilloscope.seq_group)) {};

    assert(OscilloscopeStop(&oscilloscope) == OK);

    ok();
}

void
test_oscilloscope_double_stop(void)
{
    test();

    assert(OscilloscopeStart(&oscilloscope, 1) == OK);

    // wait for the measurement to finish
    while (!OscSeqGroupReady(oscilloscope.seq_group)) {};

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

    memory.acquire = 1;
    assert(OscilloscopeStart(&oscilloscope, 1000) == MEMORY_ERROR);
    memory.acquire = 0;

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

    OscilloscopeStart(&oscilloscope, 1);
    while (!OscSeqGroupReady(oscilloscope.seq_group)) {};

    FOREACH_ADC {
        self = &oscilloscope.seq_group->osc_seq[i].ring;
        assert(RingFull(self));

        for (RingIterInit(&iter, self); iter.content; RingIterNext(&iter)) {
            page = RingIterGet(&iter);
            // head
            if (page->buffer[0] != 0xFFFFFFFF)
                fail("head (adc[%i], page[%i], (uint32_t *) buffer[0])", i, iter.read);
            // look for alignment error
            // if the alignment is off, uDMA starts at a later address
            byte_buffer = (uint8_t *) page->buffer;
            if (byte_buffer[4] == 0xFF)
                fail("alignment (adc[%i], page[%i], (uint8_t *) buffer[4]", i, iter.read);
            // measurement values
            short_buffer = (uint16_t *) page->buffer;
            for (j = 0; j < OSCILLOSCOPE_SAMPLES; j++) {
                if (short_buffer[j + 2] == 0xFFFF)
                    fail("value (adc[%i], page[%i], (uint16_t *) buffer[%i])", i, iter.read, j + 2);
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
