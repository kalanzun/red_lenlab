/*
 * clock.c
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "clock.h"

#include "driverlib/sysctl.h"
#include "driverlib/systick.h"


inline void
ClockInit(tClock *self)
{
    self->cycles_per_ms = SysCtlClockGet() / 1000;
    self->cycles_per_us = self->cycles_per_ms / 1000;

    //
    // Enable systick for timing
    //
    // allowed max value is 16,777,216
    SysTickPeriodSet(100 * self->cycles_per_ms);
    SysTickEnable();
}
