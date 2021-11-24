/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"

#include "usb_device.h"


//*****************************************************************************
//
// The control table used by the uDMA controller.  This table must be aligned
// to a 1024 byte boundary.
//
//*****************************************************************************
#pragma DATA_ALIGN(ui8ControlTable, 1024)
uint8_t ui8ControlTable[1024];


//*****************************************************************************
//
// Peripherals
//
//*****************************************************************************
const uint32_t peripherals[] = {
    SYSCTL_PERIPH_GPIOA,
    SYSCTL_PERIPH_GPIOB,
    SYSCTL_PERIPH_GPIOC,
    SYSCTL_PERIPH_GPIOD,
    SYSCTL_PERIPH_GPIOE,
    SYSCTL_PERIPH_GPIOF,

    SYSCTL_PERIPH_UDMA,
};

#define NUM_PERIPHERALS (sizeof(peripherals) / sizeof(uint32_t))


inline void
ConfigurePeripherals(void)
{
    uint8_t i;

    for (i=0; i<NUM_PERIPHERALS; i++) {
        SysCtlPeripheralEnable(peripherals[i]);
    }

    for (i=0; i<NUM_PERIPHERALS; i++) {
        while(!SysCtlPeripheralReady(peripherals[i])) {
        }
    }
}


//*****************************************************************************
//
// The error routine that is called if an ASSERT fails. (driverlib/debug.h)
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
    while (1) {
    }
}
#endif


//*****************************************************************************
//
// uDMA
//
//*****************************************************************************
void
uDMAErrorHandler(void)
{
    uDMAErrorStatusClear();
    while (1) {
    }
}


inline void
ConfigureuDMA(void)
{
    //
    // Enable the uDMA controller error interrupt.  This interrupt will occur
    // if there is a bus error during a transfer.
    //
    IntEnable(INT_UDMAERR);

    //
    // Enable the uDMA controller.
    //
    uDMAEnable();

    //
    // Point at the control table to use for channel control structures.
    //
    uDMAControlBaseSet(ui8ControlTable);

}


//*****************************************************************************
//
// This is the main application entry function.
//
//*****************************************************************************
int main(void)
{
    //
    // System Clock 80 MHz
    //
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    //
    // Configure peripherals
    //
    ConfigurePeripherals();

    //
    // Configure uDMA
    //
    ConfigureuDMA();

    //
    // Configure USB
    //
    USBDeviceInit();

    while (1) {
        USBDeviceMain();
    }
}
