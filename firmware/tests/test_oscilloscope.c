/*
 * test_oscilloscope.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "test_oscilloscope.h"

#include "oscilloscope.h"
#include "microtest.h"


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
