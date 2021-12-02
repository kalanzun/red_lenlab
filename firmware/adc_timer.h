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

#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


#define ADC_TIMER_BASE TIMER1_BASE
#define ADC_TIMER_TIMER TIMER_A


inline void
ADCTimerSetInterval(uint32_t interval)
{
    // interval in us
    TimerLoadSet64(ADC_TIMER_BASE, (uint64_t) interval * (SysCtlClockGet() / 1000 / 1000));
}


inline void
ADCTimerStart(uint32_t interval)
{
    ADCTimerSetInterval(interval);
    TimerEnable(ADC_TIMER_BASE, ADC_TIMER_TIMER);
}


inline void
ADCTimerStop(void)
{
    TimerDisable(ADC_TIMER_BASE, ADC_TIMER_TIMER);
}


inline void
ADCTimerInit(void)
{
    TimerConfigure(ADC_TIMER_BASE, TIMER_CFG_PERIODIC);
    TimerControlTrigger(ADC_TIMER_BASE, ADC_TIMER_TIMER, 1);
}


#endif /* ADC_TIMER_H_ */
