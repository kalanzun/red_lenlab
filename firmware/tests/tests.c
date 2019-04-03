/*
 * tests.c
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "driverlib/systick.h"
#include "driverlib/sysctl.h"

#include "debug.h"
#include "wait.h"


void
test_wait(void)
{
    int delta;

    delta = SysTickValueGet();
    wait(100); // 100 ms, one full round of the systick timer
    delta = abs(delta - SysTickValueGet()); // it counts down

    if (delta > 1000)
        DEBUG_PRINT("test_wait failed (%i)", delta);
    else
        DEBUG_PRINT("test_wait ok (%i)", delta);
}


void
test_log_seq(void)
{
    int delta;

    //StartLogSeq(10); // ms
    wait(100); // ms
    //StopLogSeq();

    // LogSeq counted about 10 events
    //delta = abs(GetLogSeqCount() - 10);
    delta = abs(0 - 10);
    if (delta > 2)
        DEBUG_PRINT("test_log_seq failed (%i)", delta);
}


void
tests(void)
{
    DEBUG_PRINT("start tests");
    test_wait();
    test_log_seq();
    DEBUG_PRINT("finished tests");
}
