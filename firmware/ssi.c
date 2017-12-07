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

        uDMAChannelTransferSet(UDMA_CHANNEL_SSI0TX | UDMA_PRI_SELECT,
                UDMA_MODE_BASIC,
                ssi.buffer,
                (void *)(SSI0_BASE + SSI_O_DR),
                SSI_PAYLOAD_LENGTH);

        uDMAChannelEnable(UDMA_CHANNEL_SSI0TX);
    }

}


void
SSIStart(void)
{
    //Enable SSI with desired frequency
    SSIConfigSetExpClk(
            SSI0_BASE,
            SysCtlClockGet(),
            SSI_FRF_MOTO_MODE_0,
            SSI_MODE_MASTER,
            16000000,
            16);

    SSIEnable(SSI0_BASE);

    //Enable DMA Transfer
    uDMAChannelTransferSet(UDMA_CHANNEL_SSI0TX | UDMA_PRI_SELECT,
            UDMA_MODE_BASIC,
            ssi.buffer,
            (void *)(SSI0_BASE + SSI_O_DR),
            SSI_PAYLOAD_LENGTH);

    uDMAChannelEnable(UDMA_CHANNEL_SSI0TX);

}


void
SSIStop(void)
{
    uDMAChannelDisable(UDMA_CHANNEL_SSI0TX);
}

inline void
ConfigureSSI()
{
    //
    // Configure Pins
    //
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_2);

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

void
SSIInit(void)
{
    uint32_t i;

    ConfigureSSI();

    // Rampe
    for (i = 0; i < SSI_PAYLOAD_LENGTH; i++)
        ssi.buffer[i] = (8*i) | 0x3000;

    /*
    // Konstante 1/2
    for (i = 0; i < SSI_PAYLOAD_LENGTH/2; i++)
        ssi.buffer[i] = 0 | 0x3000;
    for (i = 0; i < SSI_PAYLOAD_LENGTH/2; i++)
        ssi.buffer[512+i] = 4095 | 0x3000;
    */

    /*
    // Konstante 0x55 010101
    for (i = 0; i < SSI_PAYLOAD_LENGTH; i++)
        ssi.buffer[i] = 0x3555;
    */

    /*
    for (i = 0; i < 124; i++)
        ssi.buffer[i] = ((uint16_t) (2048 + taylor(i*26))) & 0xFFF | 0x3000;
    for (i = 0; i < 124; i++)
        ssi.buffer[124+i] = ((uint16_t) (2048 + taylor((123-i)*26))) & 0xFFF | 0x3000;
        //ssi.buffer[124+i] = ssi.buffer[123-i];
    for (i = 0; i < 124; i++)
        ssi.buffer[124+124+i] = ((uint16_t) (2048 - taylor(i*26))) & 0xFFF | 0x3000;
        //ssi.buffer[248+i] = (uint32_t) 4096 - (uint32_t) ssi.buffer[i];
    for (i = 0; i < 124; i++)
        ssi.buffer[i] = ((uint16_t) (2048 - taylor((123-i)*26))) & 0xFFF | 0x3000;
        //ssi.buffer[248+124+i] = (uint32_t) 4096 - (uint32_t) ssi.buffer[123-i];
    */

    /*
    for (i = 0; i < SSI_PAYLOAD_LENGTH; i++) {
        ssi.buffer[i] = (((uint16_t) (2048 + taylor(14 * i))) & 0xFFF) | 0x3000;
    }
    */
}

