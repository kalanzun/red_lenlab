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

//#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
//#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ssi.h"
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
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include "ssi.h"
#include "debug.h"


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
    SSIEnable(SSI0_BASE);

    SSIStartuDMAChannel(UDMA_PRI_SELECT);
    SSIStartuDMAChannel(UDMA_ALT_SELECT);

    uDMAChannelEnable(UDMA_CHANNEL_SSI0TX);
}


void
SSIStop(void)
{
    uDMAChannelDisable(UDMA_CHANNEL_SSI0TX);
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

    // Set DAC Output to zero. Needed after reset of µC
    SSIDataPut(SSI0_BASE, 0x3800);

    IntEnable(INT_SSI0);
    SSIDMAEnable(SSI0_BASE, SSI_DMA_TX);

    // **************************************************************************************
    //
    // Config of SSI with uDMA
    //
    // **************************************************************************************

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
}
/*
inline int32_t
f_mul(int32_t a, int32_t b)
{
    return (a * b) >> 11;
}

inline int32_t
taylor(int32_t x)
{
    int32_t x2 = f_mul(x, x);
    int32_t x3 = f_mul(x2, x);
    int32_t x5 = f_mul(x3, x2);
    int32_t x7 = f_mul(x5, x2);

    return (x) - (x3 / 6) + (x5 / 120) - (x7 / 5040);
}
*/
void
SSIInit(void)
{
    //uint32_t i;

    SSIConfigure();

    /*
    // Rampe steigend auf Ausgang A
    // Rampe fallend auf Ausgang B
    // Vollausschlag ist 4096 (12 bit), i läuft bis 512
    // SSI_PAYLOAD_LENGTH == 1024
    for (i = 0; i < (SSI_PAYLOAD_LENGTH >> 1); i++)
    {
        ssi.buffer[2*i] = (8*i) | 0x3000; // channel A
        ssi.buffer[2*i+1] = (8*((SSI_PAYLOAD_LENGTH >> 1)-i)) | 0xB000; // channel B
    }
    */

    /*
    // Sinus und Cosinus
    // SSI_PAYLOAD_LENGTH == 2*4*119
    for (i = 0; i < (SSI_PAYLOAD_LENGTH >> 1); i++)
    {
        ssi.buffer[2*i] = 0x3000; // channel A
        ssi.buffer[2*i+1] = 0xB000; // channel B
    }
#define OFFSET 2047
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
    for (i = 0; i < 120; i++)
    {
        ssi.buffer[2*i] = ((uint16_t) MIN(OFFSET + taylor(27*i), 4095)) | 0x3000; // channel A
        ssi.buffer[2*i+1] = ((uint16_t) MIN(OFFSET + taylor(27*(119-i)), 4095)) | 0xB000; // channel B
    }
    for (i = 0; i < 120; i++)
    {
        ssi.buffer[2*(120+i)] = ((uint16_t) MIN(OFFSET + taylor(27*(119-i)), 4095)) | 0x3000; // channel A
        ssi.buffer[2*(120+i)+1] = ((uint16_t) MAX(OFFSET - taylor(27*i), 0)) | 0xB000; // channel B
    }
    for (i = 0; i < 120; i++)
    {
        ssi.buffer[2*(2*120+i)] = ((uint16_t) MAX(OFFSET - taylor(27*i), 0)) | 0x3000; // channel A
        ssi.buffer[2*(2*120+i)+1] = ((uint16_t) MAX(OFFSET - taylor(27*(119-i)), 0)) | 0xB000; // channel B
    }
    for (i = 0; i < 120; i++)
    {
        ssi.buffer[2*(3*120+i)] = ((uint16_t) MAX(OFFSET - taylor(27*(119-i)), 0)) | 0x3000; // channel A
        ssi.buffer[2*(3*120+i)+1] = ((uint16_t) MIN(OFFSET + taylor(27*i), 4095)) | 0xB000; // channel B
    }
    */
}

