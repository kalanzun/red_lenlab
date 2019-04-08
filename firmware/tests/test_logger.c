/*
 * test_logger.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "test_logger.h"

#include "int_timer.h"
#include "logger.h"
#include "microtest.h"


void
test_log_seq()
{
    unsigned int i;
    uint32_t count;

    test();

    LogSeqGroupEnable(&logger.seq_group, 10); // ms
    wait(100); // ms
    LogSeqGroupDisable(&logger.seq_group);
    wait(10); // assert it does not run once more.

    FOREACH_ADC {
        count = logger.seq_group.log_seq[i].count;
        assert(count == 10);
    }

    ok();
}


void
test_logger_lock(void)
{
    test();

    assert(logger.lock == false);
    assert(LoggerStart(&logger, 1000) == OK);
    assert(logger.lock == true);
    assert(adc_group.lock == true);
    assert(LoggerStop(&logger) == OK);
    assert(logger.lock == false);
    assert(adc_group.lock == false);

    ok();
}


void
test_logger_double_start(void)
{
    test();

    assert(LoggerStart(&logger, 1000) == OK);
    assert(LoggerStart(&logger, 1000) == LOCK_ERROR);
    assert(LoggerStop(&logger) == OK);

    ok();
}


void
test_logger_double_stop(void)
{
    test();

    assert(LoggerStart(&logger, 1000) == OK);
    assert(LoggerStop(&logger) == OK);
    assert(LoggerStop(&logger) == LOCK_ERROR);

    ok();
}


void
test_logger_state_error(void)
{
    test();

    adc_group.lock = true;
    assert(LoggerStart(&logger, 1000) == ADC_ERROR);
    adc_group.lock = false;

    ok();
}


void
test_logger()
{
    test_log_seq();
    test_logger_lock();
    test_logger_double_start();
    test_logger_double_stop();
    test_logger_state_error();
}
