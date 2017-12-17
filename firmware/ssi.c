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
SSIStartuDMAChannel(void)
{
    uDMAChannelTransferSet(UDMA_CHANNEL_SSI0TX | UDMA_PRI_SELECT,
            UDMA_MODE_BASIC,
            ssi.buffer,
            (void *)(SSI0_BASE + SSI_O_DR),
            ssi.length);

    uDMAChannelEnable(UDMA_CHANNEL_SSI0TX);
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

    if(!uDMAChannelIsEnabled(UDMA_CHANNEL_SSI0TX))
    {
        SSIStartuDMAChannel();
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
    ASSERT(length < SSI_BUFFER_LENGTH);
    ssi.length = length;
}


uint16_t *
SSIGetBuffer(void)
{
    return ssi.buffer;
}


uint32_t
SSIGetFrequency(void)
{
    return ssi.frequency;
}


void
SSISetFrequency(uint32_t frequency)
{
    // 16 bits per channel (>> 4)
    // 2 channels (>> 1)
    uint32_t bitrate = frequency << 5;

    SSIConfigSetExpClk(
            SSI0_BASE,
            SysCtlClockGet(),
            SSI_FRF_MOTO_MODE_0,
            SSI_MODE_MASTER,
            bitrate,
            16);

    ssi.frequency = frequency;
}


void
SSIStart(void)
{
    SSIEnable(SSI0_BASE);

    SSIStartuDMAChannel();
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

    // SSI
    SSISetFrequency(500000); // 500 kHz maximum Frequency

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

