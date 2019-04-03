/*
 * wait_timer.h
 *
 */

#ifndef WAIT_TIMER_H_
#define WAIT_TIMER_H_


#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


typedef struct WaitTimer {

    uint32_t base;
    uint32_t timer;
    uint32_t int_flags;

    volatile bool ready;

} tWaitTimer;


inline void
WaitTimerIntHandler(tWaitTimer *self)
{
    TimerIntClear(self->base, self->int_flags);

    self->ready = 1;
}


inline void
WaitTimerSetInterval(tWaitTimer *self, uint32_t interval)
{
    // interval in ms
    TimerLoadSet64(self->base, (uint64_t) interval * SysCtlClockGet() / 1000);
}


inline void
WaitTimerStart(tWaitTimer *self, uint32_t interval)
{
    self->ready = 0;
    WaitTimerSetInterval(self, interval);
    TimerEnable(self->base, self->timer);
}


inline void
WaitTimerStop(tWaitTimer *self)
{
    TimerDisable(self->base, self->timer);
}


inline void
WaitTimerWait(tWaitTimer *self, uint32_t interval)
{
    WaitTimerStart(self, interval);
    while (!self->ready) {};
    WaitTimerStop(self);
}


inline void
ConfigureWaitTimer(tWaitTimer *self)
{
    TimerConfigure(self->base, TIMER_CFG_ONE_SHOT);
    TimerIntEnable(self->base, self->int_flags);

    IntEnable(INT_TIMER0A);
}


inline void
WaitTimerInit(tWaitTimer *self)
{
    self->base = TIMER0_BASE;
    self->timer = TIMER_A;
    self->int_flags = TIMER_TIMA_TIMEOUT;

    self->ready = 0;

    ConfigureWaitTimer(self);
}


#endif /* WAIT_TIMER_H_ */
