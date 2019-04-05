/*
 * tests.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "driverlib/systick.h"
#include "driverlib/sysctl.h"

#include "int_timer.h"
#include "logger.h"
#include "microtest.h"

#include "test_oscilloscope.h"
#include "test_ring.h"


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
        assertIntLess(delta, 1000);

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
test_logger_lock(void)
{
    test();

    assertFalse(logger.lock);
    assertOK(LoggerStart(&logger, 1000));
    assertTrue(logger.lock);
    assertTrue(adc_group.lock);
    assertOK(LoggerStop(&logger));
    assertFalse(logger.lock);
    assertFalse(adc_group.lock);

    ok();
}


void
test_logger_double_start(void)
{
    test();

    assertOK(LoggerStart(&logger, 1000));
    assertError(LoggerStart(&logger, 1000), LOCK_ERROR);
    assertOK(LoggerStop(&logger));

    ok();
}


void
test_logger_double_stop(void)
{
    test();

    assertOK(LoggerStart(&logger, 1000));
    assertOK(LoggerStop(&logger));
    assertError(LoggerStop(&logger), LOCK_ERROR);

    ok();
}


void
test_logger_state_error(void)
{
    tError err = OK;
    test();

    adc_group.lock = true;
    err = LoggerStart(&logger, 1000);
    adc_group.lock = false; // reset lock even if the assertion fails

    assertError(err, STATE_ERROR);
    ok();
}


void
tests(void)
{
    DEBUG_PRINT("BEGIN tests");
    /*
    test_wait();

    test_log_seq();

    test_logger_lock();
    test_logger_double_start();
    test_logger_double_stop();
    test_logger_state_error();
    */
    //test_oscilloscope();

    test_ring();
    test_oscilloscope();

    DEBUG_PRINT("END tests");
}
