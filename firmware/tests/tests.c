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
#include "logger.h"
#include "microtest.h"


void
test_wait(void)
{
    int delta;
    int i;

    test();

    for (i = 0; i < 2; i++) {

        delta = SysTickValueGet();
        wait(50); // 50 ms, half round of the systick timer
        delta = delta - SysTickValueGet(); // it counts down
        if (delta < 0) delta = delta + 8*1000*1000;

        delta = delta - 4*1000*1000;
        assertIntGreater(delta, 0);
        assertIntSmaller(delta, 1000);

    }

    ok();
}


void
test_log_seq(void)
{
    int count;

    test();

    LogSeqGroupEnable(&logger.seq_group, 10); // ms
    wait(100); // ms
    LogSeqGroupDisable(&logger.seq_group);
    wait(10); // assert it does not run once more.

    count = logger.seq_group.log_seq[0].count;
    assertIntEqual(count, 10);

    count = logger.seq_group.log_seq[1].count;
    assertIntEqual(count, 10);

    ok();
}


void
tests(void)
{
    test_wait();

    test_log_seq();
}
