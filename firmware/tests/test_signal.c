/*
 * test_signal.c
 *
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
