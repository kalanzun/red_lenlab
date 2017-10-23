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
#include "driverlib/usb.h"
#include "driverlib/udma.h"
#include "adc.h"
#include "usb_device.h"



uint16_t buffer[4][512];
bool ping;
bool pong;

volatile uint32_t adc_interrupt_counter;


void
ADCInit(void)
{
    adc_interrupt_counter = 0;
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
StartADCuDMAChannel(uint32_t ui32ChannelStructIndex)
{
    uint8_t index;

    if (ui32ChannelStructIndex & UDMA_ALT_SELECT) {
        // pong
        pong = !pong;
        index = 2+pong;
    }
    else {
        // ping
        ping = !ping;
        index = ping;
    }

    uDMAChannelTransferSet(ui32ChannelStructIndex,
            UDMA_MODE_PINGPONG,
            (void *)(ADC0_BASE + ADC_O_SSFIFO0 + (0x20 * 0)),
            buffer[index], 512);

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

    //p_DestAdr = g_pui8USBTxBufferBulk;

    //ui8Oszi_state = CONV_DONE;

    if(!uDMAChannelSizeGet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT))
    {
        SendBuffer((uint8_t *) buffer[ping]);
        StartADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);
        uDMAChannelEnable(UDMA_CHANNEL_ADC0);
        adc_interrupt_counter++;
    }
    else if(!uDMAChannelSizeGet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT))
    {
        SendBuffer((uint8_t *) buffer[2+pong]);
        StartADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);
        uDMAChannelEnable(UDMA_CHANNEL_ADC0);
        adc_interrupt_counter++;
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

     ping = 0;
     pong = 0;

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
