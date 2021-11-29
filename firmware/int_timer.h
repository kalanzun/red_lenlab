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

#ifndef INT_TIMER_H_
#define INT_TIMER_H_


#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


struct IntTimer {
    const uint32_t base;
    const uint32_t timer;

    uint32_t interval;
    uint32_t reference;

    volatile uint32_t count;
};


extern struct IntTimer int_timer;


inline void
IntTimerStart(uint32_t interval, uint32_t count, uint16_t reference)
{
    int_timer.interval = interval;
    int_timer.count = count;
    int_timer.reference = reference;

    // interval in ms
    TimerLoadSet64(int_timer.base, (uint64_t) interval * SysCtlClockGet() / 1000);
    TimerEnable(int_timer.base, int_timer.timer);
}


inline void
IntTimerStop(void)
{
    TimerDisable(int_timer.base, int_timer.timer);
}


inline void
IntTimerInit(void)
{
    TimerConfigure(int_timer.base, TIMER_CFG_PERIODIC);
    TimerIntEnable(int_timer.base, TIMER_TIMA_TIMEOUT);

    IntEnable(INT_TIMER0A);
}


#endif /* INT_TIMER_H_ */
