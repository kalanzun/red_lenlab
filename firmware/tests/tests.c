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
    test_logger();
    test_ring();
    test_oscilloscope();

    DEBUG_PRINT("END tests");
}
