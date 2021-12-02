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

#ifndef TICK_H_
#define TICK_H_


#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


#define TICK_TIMER_BASE TIMER0_BASE
#define TICK_TIMER TIMER_A
// TickInit and the interrupt handler are specific for timer 0A


struct Tick {
    uint32_t interval;
    uint16_t reference;

    volatile uint32_t count;
};


extern struct Tick tick;


inline void
TickStart(uint32_t interval, uint32_t count, uint16_t reference)
{
    tick.interval = interval;
    tick.count = count;
    tick.reference = reference;

    // interval in ms
    TimerLoadSet64(TICK_TIMER_BASE, (uint64_t) tick.interval * SysCtlClockGet() / 1000);
    TimerEnable(TICK_TIMER_BASE, TICK_TIMER);
}


inline void
TickStop(void)
{
    TimerDisable(TICK_TIMER_BASE, TICK_TIMER);
}


inline void
TickInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {
    }

    TimerConfigure(TICK_TIMER_BASE, TIMER_CFG_PERIODIC);
    TimerIntEnable(TICK_TIMER_BASE, TIMER_TIMA_TIMEOUT);

    IntEnable(INT_TIMER0A);
}


#endif /* TICK_H_ */
