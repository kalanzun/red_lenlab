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


#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"

#include "red_clock.h"
#include "usb_device.h"


struct IntTimer {
    uint32_t base;
    uint32_t timer;
};


extern struct IntTimer int_timer;


inline void
IntTimerIntHandler()
{
    TimerIntClear(int_timer.base, TIMER_TIMA_TIMEOUT);

    USBDeviceTick();
}


inline void
IntTimerStart(uint32_t interval)
{
    // interval in ms
    TimerLoadSet64(int_timer.base, (uint64_t) interval * red_clock.cycles_per_ms);
    TimerEnable(int_timer.base, int_timer.timer);
}


inline void
IntTimerStop(void)
{
    TimerDisable(int_timer.base, int_timer.timer);
}


inline void
ConfigureIntTimer(void)
{
    TimerConfigure(int_timer.base, TIMER_CFG_PERIODIC);
    TimerIntEnable(int_timer.base, TIMER_TIMA_TIMEOUT);

    IntEnable(INT_TIMER0A);
}


inline void
IntTimerInit(void)
{
    int_timer.base = TIMER0_BASE;
    int_timer.timer = TIMER_A;

    ConfigureIntTimer();
}


#endif /* INT_TIMER_H_ */
