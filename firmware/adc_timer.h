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

#ifndef ADC_TIMER_H_
#define ADC_TIMER_H_


#include "inc/hw_memmap.h"
#include "driverlib/timer.h"

#include "clock.h"
#include "debug.h"
#include "error.h"


typedef struct ADCTimer {

    uint32_t base;
    uint32_t timer;

} tADCTimer;


inline void
ADCTimerSetInterval(tADCTimer *self, uint32_t interval)
{
    // interval in us
    TimerLoadSet64(self->base, (uint64_t) interval * red_clock.cycles_per_us);
}


inline void
ADCTimerStart(tADCTimer *self, uint32_t interval)
{
    ADCTimerSetInterval(self, interval);
    TimerEnable(self->base, self->timer);
}


inline void
ADCTimerStop(tADCTimer *self)
{
    TimerDisable(self->base, self->timer);
}


inline void
ConfigureADCTimer(tADCTimer *self)
{
    TimerConfigure(self->base, TIMER_CFG_PERIODIC);
    TimerControlTrigger(self->base, self->timer, 1);
}


inline void
ADCTimerInit(tADCTimer *self)
{
    self->base = TIMER1_BASE;
    self->timer = TIMER_A;

    ConfigureADCTimer(self);
}

#endif /* ADC_TIMER_H_ */
