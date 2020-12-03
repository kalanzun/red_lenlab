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

#ifndef INT_TIMER_H_
#define INT_TIMER_H_


#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

#include "clock.h"


typedef struct IntTimer {

    uint32_t base;
    uint32_t timer;
    uint32_t int_flags;

    volatile bool ready;

} tIntTimer;


extern tIntTimer int_timer;


inline void
IntTimerIntHandler(tIntTimer *self)
{
    TimerIntClear(self->base, self->int_flags);

    self->ready = 1;
}


inline void
IntTimerStart(tIntTimer *self, uint32_t interval)
{
    // interval in ms
    self->ready = 0;
    TimerLoadSet64(self->base, (uint64_t) interval * clock.cycles_per_ms);
    TimerEnable(self->base, self->timer);
}


inline void
IntTimerStop(tIntTimer *self)
{
    TimerDisable(self->base, self->timer);
}


inline void
IntTimerWait(tIntTimer *self, uint32_t interval)
{
    IntTimerStart(self, interval);
    while (!self->ready) {};
    IntTimerStop(self);
}


inline void
ConfigureIntTimer(tIntTimer *self)
{
    TimerConfigure(self->base, TIMER_CFG_ONE_SHOT);
    TimerIntEnable(self->base, self->int_flags);

    IntEnable(INT_TIMER0A);
}


inline void
IntTimerInit(tIntTimer *self)
{
    self->base = TIMER0_BASE;
    self->timer = TIMER_A;
    self->int_flags = TIMER_TIMA_TIMEOUT;

    self->ready = 0;

    ConfigureIntTimer(self);
}


inline void
wait(uint32_t interval)
{
    // interval in ms
    IntTimerWait(&int_timer, interval);
}


#endif /* INT_TIMER_H_ */
