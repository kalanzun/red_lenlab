/*
 * timer.c
 *
 *  Created on: 11.11.2017
 *      Author: Christoph
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
#include "debug.h"
#include "timer.h"
#include "adc.h"


tTimer timer;


void
Timer0IntHandler(void)
{
    DEBUG_PRINT("timer int\n");
    TimerIntClear(TIMER0_BASE, TIMER_A);

    ADCStartSingle(timer.time);
    timer.time += timer.interval;
}

void
TimerStart()
{
    ASSERT(timer.locked);

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

    if (timer.locked && !timer.active) {
        timer.interval = interval;
        load_value = interval * (SysCtlClockGet() / 1000);
        DEBUG_PRINT("%d\n", interval);
        TimerLoadSet(TIMER0_BASE, TIMER_A, load_value);
    }
}

bool
TimerAcquire(void)
{
    if (timer.locked)
        return 0;

    if (!ADCAcquire())
        return 0;

    timer.locked = 1;
    return 1;
}

void
TimerRelease(void)
{
    if (timer.locked) {
        ADCRelease();

        timer.locked = 0;
    }
}

inline void
ConfigureTimer(void)
{
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
}

void
TimerInit(void)
{
    timer.locked = 0;
    timer.active = 0;
    timer.interval = 1000;
    timer.time = 0;

    ConfigureTimer();
}
