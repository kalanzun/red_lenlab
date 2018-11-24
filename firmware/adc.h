/*
 * adc.h
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

#define ADC_SAMPLES 500


typedef struct ADC {

    uint32_t base;
    uint32_t channel;
    uint32_t gpio_base;
    uint32_t gpio_pin;
    uint32_t udma_channel;

    tADCTimer *timer;

} tADC;


typedef struct ADCGroup {

    tADC adc[ADC_GROUP_SIZE];

    tADCTimer timer;

    volatile unsigned char lock;

} tADCGroup;


extern tADCGroup adc_group;


inline tError
ADCGroupLock(tADCGroup *self)
{
    if (self->lock) return LOCK_ERROR;
    self->lock = 1;
    return OK;
}


inline void
ADCGroupUnlock(tADCGroup *self)
{
    self->lock = 0;
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
    int i;

    ConfigureADC0(&self->adc[0]);
    ConfigureADC1(&self->adc[1]);

    ADCTimerInit(&self->timer);

    FOREACH_ADC self->adc[i].timer = &self->timer;

    self->lock = 0;
}


/*
void
ADCxStart(tADCx *self, tPage *ping, tPage *pong)
{

    uDMAChannelAttributeDisable(self->udma_channel,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
        UDMA_ATTR_HIGH_PRIORITY |
        UDMA_ATTR_REQMASK);

    uDMAChannelControlSet(self->udma_channel | UDMA_PRI_SELECT,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_4);

    uDMAChannelControlSet(self->udma_channel | UDMA_ALT_SELECT,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_4);

    uDMAChannelTransferSet(self->udma_channel | UDMA_PRI_SELECT,
        UDMA_MODE_PINGPONG,
        (void *)(self->adc_base + ADC_O_SSFIFO0),
        ping->buffer+24, ADC_SAMPLES);

    self->ping_enable = 1;

    uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
        UDMA_MODE_PINGPONG,
        (void *)(self->adc_base + ADC_O_SSFIFO0),
        pong->buffer+24, ADC_SAMPLES);

    self->pong_enable = 1;

    ADCSequenceEnable(self->adc_base, 0);
    ADCIntDisable(self->adc_base, 3); // either ADC interrupt or DMA
    uDMAChannelEnable(self->udma_channel);
    IntEnable(self->adc_int);
}


inline void
ADCxDMAEnable(tADCx *self)
{
    uDMAChannelEnable(self->udma_channel);
    // TODO IntEnable?
}


inline void
ADCDMAEnable(tADC *self)
{
    for (int i = 0; i < ADCX; ++i) ADCxDMAEnable(self->adcx[i]);
}


uint32_t ADCLoggerStart();
bool ADCLoggerReady();
void ADCLoggerGet(uint32_t *channel0, uint32_t *channel1);
void ADCLoggerRelease();
void ADCLoggerStop();

uint32_t ADCStart(uint32_t length, uint32_t samplerate);
bool ADCReady();
tRing *ADCGetRing(bool channel);

void ADCInit();
*/
#endif /* ADC_H_ */
