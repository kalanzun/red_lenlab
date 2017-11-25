/*
 * logger.c
 *

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <stdbool.h>
#include <stdint.h>
#include "driverlib/debug.h"
#include "timer.h"
#include "logger.h"
#include "debug.h"


tLogger logger;


void
LoggerStart(void)
{
    DEBUG_PRINT("start\n");

    if (logger.active)
        return;

    logger.active = 1;

    TimerSetInterval(logger.interval);
    TimerStart();
}

void
LoggerStop(void)
{
    DEBUG_PRINT("stop\n");
    if (logger.active) {
        TimerStop();
        logger.active = 0;
    }
}

void
LoggerSetInterval(uint32_t interval)
{
    DEBUG_PRINT("logger setInterval %d", interval);
    logger.interval = interval;
}

void
LoggerInit(void)
{
    logger.interval = 1000;
}
