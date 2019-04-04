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
    wait(50); // 50 ms, half round of the systick timer
    delta = delta - SysTickValueGet(); // it counts down
    if (delta < 0) delta = delta + 8*1000*1000;

    delta = delta - 4*1000*1000;
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
