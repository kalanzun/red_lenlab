/*
 * adc.c
 *
 *  Created on: 23.10.2017
 *      Author: Christoph
 */


//#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
//#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_adc.h"
//#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
//#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
//#include "driverlib/uart.h"
#include "driverlib/gpio.h"
//#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/usb.h"
#include "driverlib/udma.h"
#include "adc.h"
#include "usb_device.h"
#include "adc_queue.h"


tADC adc;


inline void
StartADCuDMAChannel(uint32_t ui32ChannelStructIndex)
{
    ASSERT(!ADCQueueFull(&adc.adc_queue)); // TODO Error Handling

    uDMAChannelTransferSet(ui32ChannelStructIndex,
            UDMA_MODE_PINGPONG,
            (void *)(ADC0_BASE + ADC_O_SSFIFO0 + (0x20 * 0)),
            ADCQueueAcquire(&adc.adc_queue)->payload, ADC_PAYLOAD_LENGTH);

}


//*****************************************************************************
//
// The interrupt handler for ADC0, sequence 0.  This interrupt will occur when
// a DMA transfer is complete using the first sequencer on ADC0.  It will also
// be triggered if the peripheral signals an error.
//
//*****************************************************************************
void
ADC0IntHandler(void)
{
    //
    // Clear the ADC interrupt
    //
    ADCIntClear(ADC0_BASE, 0);

    if(!uDMAChannelSizeGet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT))
    {
        ADCQueueWrite(&adc.adc_queue);
        StartADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);
        uDMAChannelEnable(UDMA_CHANNEL_ADC0);
    }
    else if(!uDMAChannelSizeGet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT))
    {
        ADCQueueWrite(&adc.adc_queue);
        StartADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);
        uDMAChannelEnable(UDMA_CHANNEL_ADC0);
    }
    else
    {
        // normal ADC interrupt, not uDMA interrupt / what???
        //g_ulIntOsziCount++;
        //while (1) {};
    }
}


void
ADCStart(void)
{
    uDMAChannelAttributeDisable(UDMA_CHANNEL_ADC0,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
        UDMA_ATTR_HIGH_PRIORITY |
        UDMA_ATTR_REQMASK);

    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_4);

    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_4);

    StartADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);
    StartADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);

    uDMAChannelEnable(UDMA_CHANNEL_ADC0);
    ADCSequenceEnable(ADC0_BASE, 0);

    //uDMAChannelEnable(UDMA_CH24_ADC1_0);
    IntEnable(INT_ADC0SS0);
    //IntEnable(INT_ADC1SS0);
    //g_bPingReadyADC0 = true;
    //g_bPongReadyADC0 = true;
}


void
ADCMain(void)
{

}


inline void
ConfigureADC(tADC *self)
{
    //
    // Configure Pins
    //
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);

    // Set the ADC Sequence to trigger always (that is 1 MHz)
    // and to generate an interrupt every 4 samples.
    // This is an arbitration size of 4 for the uDMA transfer
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_ALWAYS, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH1 | ADC_CTL_IE);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);

    /*  TODO: second ADC Module
    ROM_ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_ALWAYS, 0);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 3, ADC_CTL_CH2 | ADC_CTL_IE);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 4, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 5, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 6, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 7, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);
     */


    //
    // Enable the sequencer
    //
    ADCSequenceEnable(ADC0_BASE, 0);

    //ROM_ADCSequenceEnable(ADC1_BASE, 0);

    //
    // Enable sequencer interrupts
    //
    ADCIntEnable(ADC0_BASE, 0);
    //ROM_ADCIntEnable(ADC1_BASE, 0);
}


void
ADCInit(void)
{
    ADCQueueInit(&adc.adc_queue);

    ConfigureADC(&adc);
}

