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

#include "int_timer.h"

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"

#include "reply_handler.h"


struct IntTimer int_timer = {
    .base = TIMER0_BASE,
    .timer = TIMER_A
};


void
Timer0AIntHandler(void)
{
    struct Message *reply;

    TimerIntClear(int_timer.base, TIMER_TIMA_TIMEOUT);

    if (int_timer.count == 0) return;
    if (--int_timer.count == 0) IntTimerStop();

    ASSERT(reply_queue.has_space);
    reply = RingAcquire(&reply_queue);

    reply->reply = Tick;
    reply->type = nullType;
    reply->argument = (uint16_t) int_timer.count;

    RingWrite(&reply_queue);
}
