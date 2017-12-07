/*
 * timer.c
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
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "debug.h"
#include "timer.h"
#include "adc.h"


tTimer timer;


void
Timer0IntHandler(void)
{
    static int tick = 0;
    //DEBUG_PRINT("timer int\n");
    TimerIntClear(TIMER0_BASE, TIMER_A);

    //ADCStartSingle(timer.time);

    if (tick) {
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_PIN_0);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        tick = 0;
    }
    else {
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        tick = 1;
    }

    //tick = ~tick;

    timer.time += timer.interval;
}

void
TimerStart()
{
    if (timer.active)
        return;

    timer.time = 0;

    TimerEnable(TIMER0_BASE, TIMER_A);
    timer.active = 1;
}

void
TimerStop(void)
{
    if (timer.active) {
        TimerDisable(TIMER0_BASE, TIMER_A);
        timer.active = 0;
    }
}

void
TimerSetInterval(uint32_t interval)
{
    uint32_t load_value;

    if (!timer.active) {
        timer.interval = interval;
        load_value = interval * (SysCtlClockGet() / 1000 / 1000); // us // ms
        DEBUG_PRINT("%d\n", interval);
        TimerLoadSet(TIMER0_BASE, TIMER_A, load_value);
    }
}

inline void
ConfigureTimer(void)
{
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
}

void
TimerInit(void)
{
    timer.active = 0;
    timer.interval = 200;
    timer.time = 0;

    ConfigureTimer();
}
