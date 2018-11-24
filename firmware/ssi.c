/*
 * ssi.c
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

#include "ssi.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ssi.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/udma.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"


tSSI ssi;


inline void
SSIStartuDMAChannel(uint32_t select)
{
    uDMAChannelTransferSet(UDMA_CHANNEL_SSI0TX | select,
            UDMA_MODE_PINGPONG,
            ssi.buffer,
            (void *)(SSI0_BASE + SSI_O_DR),
            ssi.length);
}


//*****************************************************************************
//
// The interrupt handler for ADC0, sequence 0.  This interrupt will occur when
// a DMA transfer is complete using the first sequencer on ADC0.  It will also
// be triggered if the peripheral signals an error.
//
//*****************************************************************************
void
SSI0IntHandler(void)
{
    uint32_t ui32Status;

    ui32Status = SSIIntStatus(SSI0_BASE, 1);

    SSIIntClear(SSI0_BASE, ui32Status);

    if(uDMAChannelModeGet(UDMA_CHANNEL_SSI0TX | UDMA_PRI_SELECT) == UDMA_MODE_STOP)
    {
        SSIStartuDMAChannel(UDMA_PRI_SELECT);
    }
    else if(uDMAChannelModeGet(UDMA_CHANNEL_SSI0TX | UDMA_ALT_SELECT) == UDMA_MODE_STOP)
    {
        SSIStartuDMAChannel(UDMA_ALT_SELECT);
    }
}


uint32_t
SSIGetLength(void)
{
    return ssi.length;
}


void
SSISetLength(uint32_t length)
{
    ASSERT(length <= SSI_BUFFER_LENGTH);
    ssi.length = length;
}


uint16_t *
SSIGetBuffer(void)
{
    return ssi.buffer;
}


void
SSISetDivider(uint8_t predivider, uint8_t divider)
{
    // SSInClk = SysClk / (CPSDVSR * (1 + SCR))
    // SSInClk = SysClk / predivider / divider
    // SSISetDivider takes care of the +1

    // SSI überträgt 16 bit Daten und dann ein Frame End Singnal (FSS)
    // Das Frame End dauert 1,5 Clock-Schritte, so dass die Datenrate
    // um den Faktor 16/17,5 verlangsamt ist

    //
    // Set the clock predivider.
    //
    HWREG(SSI0_BASE + SSI_O_CPSR) = predivider;

    //
    // Set protocol and clock rate.
    //
    HWREG(SSI0_BASE + SSI_O_CR0) = ((divider-1) << 8) | (16 - 1); // protocol 0 and 16 bits
}


void
SSIStart(void)
{
    SSIStartuDMAChannel(UDMA_PRI_SELECT);
    SSIStartuDMAChannel(UDMA_ALT_SELECT);

    uDMAChannelEnable(UDMA_CHANNEL_SSI0TX);
}


void
SSIStop(void)
{
    uDMAChannelDisable(UDMA_CHANNEL_SSI0TX);

    // Set DAC Output to 1/2.
    SSIDataPut(SSI0_BASE, 0x3800); // channel A
    SSIDataPut(SSI0_BASE, 0xB800); // channel B
}


inline void
SSIConfigure()
{
    //
    // Configure Pins
    //
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_2);

    //
    // Set the mode.
    //
    HWREG(SSI0_BASE + SSI_O_CR1) = 0; // master mode

    //
    // Configure SSI Frequency and Protocol
    //
    SSISetDivider(2, 2);

    SSIEnable(SSI0_BASE);

    SSIStop();

    //
    // Configure SSI with uDMA
    //
    uDMAChannelAttributeDisable(UDMA_CHANNEL_SSI0TX,
            UDMA_ATTR_ALTSELECT |
            UDMA_ATTR_HIGH_PRIORITY |
            UDMA_ATTR_REQMASK);

    uDMAChannelAttributeEnable(UDMA_CHANNEL_SSI0TX, UDMA_ATTR_USEBURST);

    uDMAChannelControlSet(UDMA_CHANNEL_SSI0TX | UDMA_PRI_SELECT,
            UDMA_SIZE_16 | UDMA_SRC_INC_16 | UDMA_DST_INC_NONE |
            UDMA_ARB_4);
    uDMAChannelControlSet(UDMA_CHANNEL_SSI0TX | UDMA_ALT_SELECT,
            UDMA_SIZE_16 | UDMA_SRC_INC_16 | UDMA_DST_INC_NONE |
            UDMA_ARB_4);

    IntEnable(INT_SSI0);
    SSIDMAEnable(SSI0_BASE, SSI_DMA_TX);

    // DMA enabled, but not running (DMAChannelEnable not called)
}

void
SSIInit(void)
{
    SSIConfigure();
}

