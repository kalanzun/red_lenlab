/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>

#include "tests/test_logger.h"

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
    // These 101ms may cause the test to fail, if the firmware changes timing behaviour
    wait(101); // ms
    LogSeqGroupDisable(&logger.seq_group);
    wait(11); // assert it does not run once more.

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
test_logger_adc_error(void)
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
    test_logger_adc_error();
}
