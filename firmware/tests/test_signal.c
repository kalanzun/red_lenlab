/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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

#include "tests/test_signal.h"

#include "signal.h"
#include "ssi.h"
#include "microtest.h"


void
test_signal_lock()
{
    test();

    assert(signal.lock == false);
    assert(SignalStart(&signal) == OK);
    assert(signal.lock == true);
    assert(SignalStop(&signal) == OK);
    assert(signal.lock == false);

    ok();
}


void
test_signal_double_start()
{
    test();

    assert(SignalStart(&signal) == OK);
    assert(SignalStart(&signal) == LOCK_ERROR);
    assert(SignalStop(&signal) == OK);

    ok();
}


void
test_signal_double_stop()
{
    test();

    assert(SignalStart(&signal) == OK);
    assert(SignalStop(&signal) == OK);
    assert(SignalStop(&signal) == LOCK_ERROR);

    ok();
}


void
test_signal_set_sine()
{
    unsigned int i;
    uint16_t *buffer;
    test();

    buffer = SSIGetBuffer(&ssi);
    for (i = 0; i < SSI_BUFFER_LENGTH; i++)
        buffer[i] = 0xFFFF;

    SignalSetSine(&signal, 1, 1, 1, 1, 1);

    for (i = 0; i < SSI_BUFFER_LENGTH; i++)
        if (buffer[i] == 0xFFFF) {
            fail("signal (uint16_t *) buffer[%i]", i);
            return;
        }

    ok();
}


void
test_signal()
{
    test_signal_lock();
    test_signal_double_start();
    test_signal_double_stop();
    test_signal_set_sine();
}
