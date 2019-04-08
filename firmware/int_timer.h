/*
 * int_timer.h
 *
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
