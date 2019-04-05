/*
 * test_oscilloscope.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "test_oscilloscope.h"

#include "oscilloscope.h"
#include "microtest.h"
#include "int_timer.h"


void
test_oscilloscope_lock(void)
{
    test();

    assertFalse(oscilloscope.lock);
    assertOK(OscilloscopeStart(&oscilloscope, 1000));
    assertTrue(oscilloscope.lock);
    assertTrue(adc_group.lock);
    assertOK(OscilloscopeStop(&oscilloscope));
    assertFalse(oscilloscope.lock);
    assertFalse(adc_group.lock);

    ok();
}


void
test_oscilloscope_double_start(void)
{
    test();

    assertOK(OscilloscopeStart(&oscilloscope, 1000));
    assertError(OscilloscopeStart(&oscilloscope, 1000), LOCK_ERROR);
    assertOK(OscilloscopeStop(&oscilloscope));

    ok();
}


void
test_oscilloscope_double_stop(void)
{
    test();

    assertOK(OscilloscopeStart(&oscilloscope, 1000));
    assertOK(OscilloscopeStop(&oscilloscope));
    assertError(OscilloscopeStop(&oscilloscope), LOCK_ERROR);

    ok();
}


void
test_oscilloscope_state_error(void)
{
    tError err = OK;
    test();

    adc_group.lock = true;
    err = OscilloscopeStart(&oscilloscope, 1000);
    adc_group.lock = false; // reset lock even if the assertion fails

    assertError(err, STATE_ERROR);
    ok();
}


void
test_oscilloscope()
{
    int i, j;
    tRing *ring;
    tPage *page;
    tRingIter iter;

    test();

    for (i = 0; i < OSCILLOSCOPE_MEMORY_LENGTH; i++) {
        for (j = 0; j < PAGE_LENGTH; j++) {
            oscilloscope.memory[i].buffer[j] = 0;
        }
    }


    assertOK(OscilloscopeStart(&oscilloscope, 1));
    while (!OscSeqGroupReady(&oscilloscope.seq_group)) {};
    ADCTimerStop(&oscilloscope.adc_group->timer); // DMA disables itself, the timer is left running

    wait(100);

    for (i = 0; i < 20; i++) {
        for (j = 0; j < 24; j++) {
            if (!(oscilloscope.memory[i].buffer[j] == 0)) {
                fail("(%i, %i)", i, j);
                //return;
            }
        }
        for (j = 24; j < PAGE_LENGTH; j+=2) {
            if (*((uint16_t *) oscilloscope.memory[i].buffer+j) == 0) {
                fail("(%i, %i)", i, j);
            }
            /*
            if (j % 2 == 0 && oscilloscope.memory[i].buffer[j] != 0) {
                fail("(%i, %i)", i, j);
                return;
            }
            else if (j % 2 == 1 && oscilloscope.memory[i].buffer[j] == 0) {
                fail("(%i, %i)", i, j);
                //return;
            }
            */
        }
    }
    for (i = 20; i < OSCILLOSCOPE_MEMORY_LENGTH; i++) {
        for (j = 0; j < PAGE_LENGTH; j++) {
            if (!(oscilloscope.memory[i].buffer[j] == 0)) {
                fail("not empty");
                return;
            }
        }
    }

    /*
    FOREACH_ADC {

        ring = &oscilloscope.seq_group.osc_seq[i].ring;

        for (RingIterInit(&iter, ring); iter.content; RingIterNext(&iter))
        {
            page = RingIterGet(&iter);
            for (j = 0; j < PAGE_LENGTH; j++)
            {
                if (!(page->buffer[j] > 0)) {
                    fail("page buffer[%i] is not greater 0: %i", i, page->buffer[j]);
                    return;
                }
            }
        }

    }
    */

    assertOK(OscilloscopeStop(&oscilloscope));

    ok();
}
