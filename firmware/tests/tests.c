/*
 * tests.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "driverlib/systick.h"

#include "int_timer.h"
#include "microtest.h"

#include "tests/test_logger.h"
#include "tests/test_oscilloscope.h"
#include "tests/test_ring.h"
#include "tests/test_trigger.h"
#include "tests/test_signal.h"


void
test_wait()
{
    unsigned int i;
    int delta;

    test();

    for (i = 0; i < 2; i++) {
        delta = SysTickValueGet();
        wait(50); // 50 ms, half round of the systick timer
        delta = delta - SysTickValueGet(); // it counts down
        if (delta < 0) delta = delta + 8*1000*1000;

        delta = delta - 4*1000*1000;
        assert(0 < delta && delta < 1000);
    }

    ok();
}



void
tests(void)
{
    DEBUG_PRINT("BEGIN tests");

    test_wait();
    test_ring();

    test_signal();

    test_logger();
    test_oscilloscope();
    test_trigger();

    test_log_seq();
    test_oscilloscope_measurement();
    test_trigger_measurement();
    test_log_seq();
    test_log_seq();
    test_trigger_measurement();
    test_trigger_measurement();
    test_oscilloscope_measurement();
    test_oscilloscope_measurement();
    test_log_seq();

    DEBUG_PRINT("END tests");
}
