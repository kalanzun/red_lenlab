/*
 * test_oscilloscope.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "test_oscilloscope.h"

#include "oscilloscope.h"
#include "memory.h"
#include "microtest.h"
#include "int_timer.h"

/*
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
*/
/*
void
test_oscilloscope_double_start(void)
{
    test();

    assertOK(OscilloscopeStart(&oscilloscope, 1000));
    assertError(OscilloscopeStart(&oscilloscope, 1000), LOCK_ERROR);
    assertOK(OscilloscopeStop(&oscilloscope));

    ok();
}
*/
/*
void
test_oscilloscope_double_stop(void)
{
    test();

    assertOK(OscilloscopeStart(&oscilloscope, 1000));
    assertOK(OscilloscopeStop(&oscilloscope));
    assertError(OscilloscopeStop(&oscilloscope), LOCK_ERROR);

    ok();
}
*/
/*
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
*/

void
test_oscilloscope_measurement()
{
    int i, j;
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
    while (!OscSeqGroupReady(&oscilloscope.seq_group)) {};

    FOREACH_ADC {
        self = &oscilloscope.seq_group.osc_seq[i].ring;
        assert(RingFull(self));

        for (RingIterInit(&iter, self); iter.content; RingIterNext(&iter)) {
            page = RingIterGet(&iter);
            // head
            for (j = 0; j < 6; j++) {
                if (page->buffer[j] != 0xFFFFFFFF)
                    fail("head (%i, %i, %i)", i, iter.read, j);
            }
            // look for alignment error
            // if the alignment is off, uDMA starts at a later address
            if (*(uint8_t *) (page->buffer + 6) == 0xFF)
                fail("alignment (%i, %i)", i, iter.read);
            // measurement values
            for (j = 0; j < OSCILLOSCOPE_SAMPLES; j++) {
                value = (uint16_t *) (page->buffer + 6) + j;
                if (*value == 0xFFFF)
                    fail("value (%i, %i, %i)", i, iter.read, j);
            }
        }
    }

    OscilloscopeStop(&oscilloscope);
    ok();
}

void
test_oscilloscope()
{
    test_oscilloscope_measurement();
}
