/*
 * tests.c
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "driverlib/systick.h"
#include "driverlib/sysctl.h"

#include "debug.h"
#include "int_timer.h"
#include "microtest.h"


void
test_wait(void)
{
    int delta;

    test();

    delta = SysTickValueGet();
    wait(100); // 100 ms, one full round of the systick timer
    delta = delta - SysTickValueGet(); // it counts down

    assertIntGreater(delta, 0);
    assertIntSmaller(delta, 1000);

    ok();
}


void
test_log_seq(void)
{
    int count;

    test();

    //StartLogSeq(10); // ms
    wait(100); // ms
    //StopLogSeq();

    // LogSeq counted about 10 events
    //delta = abs(GetLogSeqCount() - 10);
    count = 0;

    assertIntEqual(count, 10);

    ok();
}


void
tests(void)
{
    test_wait();

    test_log_seq();
}
