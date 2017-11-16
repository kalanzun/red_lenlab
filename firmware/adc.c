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
#include "debug.h"


tADC adc;


inline void
StartPingPongADCuDMAChannel(uint32_t ui32ChannelStructIndex)
{
    ASSERT(!ADCQueueFull(&adc.adc_queue)); // TODO Error Handling

    uDMAChannelTransferSet(ui32ChannelStructIndex,
            UDMA_MODE_PINGPONG,
            (void *)(ADC0_BASE + ADC_O_SSFIFO0),
            ADCQueueAcquire(&adc.adc_queue)->payload, ADC_PAYLOAD_LENGTH);

}


inline void
StartBasicADCuDMAChannel(uint32_t ui32ChannelStructIndex, uint32_t time)
{
    tADCEvent *event;

    ASSERT(!ADCQueueFull(&adc.adc_queue)); // TODO Error Handling
    event = ADCQueueAcquire(&adc.adc_queue);

    *((uint32_t *) event->payload) = time;

    uDMAChannelTransferSet(ui32ChannelStructIndex,
            UDMA_MODE_BASIC,
            (void *)(ADC0_BASE + ADC_O_SSFIFO0),
            event->payload + 4, ADC_PAYLOAD_LENGTH - 4);
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

    if (adc.pingpong)
    {
        if(!uDMAChannelSizeGet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT))
        {
            //DEBUG_PRINT("PRI\n");
            ADCQueueWrite(&adc.adc_queue);
            // Achtung: Beim Beenden leaken wir hier adc_queue.acquire
            //logger operation one shot DMA
            StartPingPongADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);
            // damit der interrupt das nächste Mal nach UDMA_ALT_SELECT springt muss das DMA hier neu konfiguriert werden, auch wenn dann nach ALT angehalten werden soll.
            //uDMAChannelEnable(UDMA_CHANNEL_ADC0); // TODO event. nicht nötig, bitte probieren
        }
        else if(!uDMAChannelSizeGet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT))
        {
            //DEBUG_PRINT("ALT\n");
            ADCQueueWrite(&adc.adc_queue);
            //logger operation one shot DMA
            StartPingPongADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);
            //uDMAChannelEnable(UDMA_CHANNEL_ADC0);
        }
        //else
        //{
            // normal ADC interrupt, not uDMA interrupt / what???
            // does happen if IntEnable is with ADCStart instead of ADCInit.
            //ASSERT(0);
        //}
    }
    else if (adc.basic) // single shot
    {
        if(!uDMAChannelSizeGet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT))
        {
            DEBUG_PRINT("SGN\n");
            ADCQueueWrite(&adc.adc_queue);
            ADCSequenceDisable(ADC0_BASE, 0);
            adc.basic = 0;
        }
        //else
        //{
            // normal ADC interrupt, not uDMA interrupt / what???
            // does happen if IntEnable is with ADCStart instead of ADCInit.
            //ASSERT(0);
        //}
    }
}


void
ADCStart(void)
{
    if (!adc.locked && adc.basic && adc.pingpong)
        return;

    adc.pingpong = 1;

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

    StartPingPongADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);
    StartPingPongADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);

    uDMAChannelEnable(UDMA_CHANNEL_ADC0);
    ADCSequenceEnable(ADC0_BASE, 0);

    IntEnable(INT_ADC0SS0);
}


void
ADCStop(void)
{

}


void
ADCStartSingle(uint32_t time)
{
    if (!adc.locked && adc.basic && adc.pingpong)
        return;

    adc.basic = 1;

    uDMAChannelAttributeDisable(UDMA_CHANNEL_ADC0,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
        UDMA_ATTR_HIGH_PRIORITY |
        UDMA_ATTR_REQMASK);

    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_4);

    StartBasicADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT, time);

    uDMAChannelEnable(UDMA_CHANNEL_ADC0);
    ADCSequenceEnable(ADC0_BASE, 0);

    IntEnable(INT_ADC0SS0);
}


bool
ADCAcquire(void)
{
    if (adc.locked)
        return 0;

    adc.locked = 1;
    return 1;
}


void
ADCRelease(void)
{
    if (adc.locked) {
        adc.locked = 0;
    }
}


inline void
ConfigureADC()
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
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH4 | ADC_CTL_IE);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH4 | ADC_CTL_IE | ADC_CTL_END);

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
    adc.locked = 0;
    adc.pingpong = 0;
    adc.basic = 0;
    ADCQueueInit(&adc.adc_queue);

    ConfigureADC();
}
