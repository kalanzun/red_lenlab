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
