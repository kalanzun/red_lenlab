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

#include "tick.h"

#include "driverlib/debug.h"

#include "message.h"
#include "reply_handler.h"


struct Tick tick;


void
Timer0AIntHandler(void)
{
    struct Message *reply;

    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if (tick.count == 0) return;

    if (--tick.count == 0) TickStop();

    if (reply_queue.has_space) {
        reply = RingAcquire(&reply_queue);
        setReply(reply, Tick, IntArray, tick.reference);
        reply->size = 8;
        setInt(reply, 0, tick.count);
        RingWrite(&reply_queue);
    }
    else {
        TickStop();
    }
}
