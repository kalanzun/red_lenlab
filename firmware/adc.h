/*
 * adc.h
 *
 */

#ifndef ADC_H_
#define ADC_H_


#include "adc_timer.h"

#include "inc/hw_adc.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/udma.h"


#define ADC_GROUP_SIZE 2
#define FOREACH_ADC for (i = 0; i < ADC_GROUP_SIZE; ++i)


typedef struct ADC {

    uint32_t base;
    uint32_t channel;
    uint32_t gpio_base;
    uint32_t gpio_pin;

} tADC;


typedef struct ADCGroup {

    tADC adc[ADC_GROUP_SIZE];

    tADCTimer timer;

    unsigned char lock;

} tADCGroup;


extern tADCGroup adc_group;


inline void
ADCGroupLock(tADCGroup *self)
{
    self->lock = 1;
}


inline void
ADCGroupUnlock(tADCGroup *self)
{
    self->lock = 0;
}


inline void
ADCGroupSetHardwareOversample(tADCGroup *self, uint32_t samplerate)
{
    int i;

    FOREACH_ADC ADCHardwareOversampleConfigure(self->adc[i].base, 1<<samplerate);
}


inline void
ConfigureADC(tADC *self)
{
    GPIOPinTypeADC(self->gpio_base, self->gpio_pin);

    ADCPhaseDelaySet(self->base, ADC_PHASE_0);
    ADCHardwareOversampleConfigure(self->base, 1);
}


inline void
ConfigureADC0(tADC *self)
{
    self->base = ADC0_BASE;
    self->channel = ADC_CTL_CH7;
    self->gpio_base = GPIO_PORTD_BASE;
    self->gpio_pin = GPIO_PIN_0;

    ConfigureADC(self);
}


inline void
ConfigureADC1(tADC *self)
{
    self->base = ADC1_BASE;
    self->channel = ADC_CTL_CH6;
    self->gpio_base = GPIO_PORTD_BASE;
    self->gpio_pin = GPIO_PIN_1;

    uDMAChannelAssign(UDMA_CH24_ADC1_0);

    ConfigureADC(self);
}


inline void
ADCGroupInit(tADCGroup *self)
{
    ConfigureADC0(&self->adc[0]);
    ConfigureADC1(&self->adc[1]);

    ADCTimerInit(&self->timer);

    self->lock = 0;
}


#endif /* ADC_H_ */
