/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
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
