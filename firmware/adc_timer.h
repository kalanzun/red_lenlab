/*
 * adc_timer.h
 *
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
    TimerLoadSet64(self->base, (uint64_t) interval * clock.cycles_per_us);
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
