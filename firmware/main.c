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

#include "adc_group.h"
#include "command_handler.h"
#include "logger.h"
#include "oscilloscope.h"
#include "reply_handler.h"
#include "tick.h"
#include "usb_device.h"


//*****************************************************************************
//
// The control table used by the uDMA controller.  This table must be aligned
// to a 1024 byte boundary.
//
//*****************************************************************************
static uint8_t ControlTable[1024] __attribute__ ((aligned(1024)));


//*****************************************************************************
//
// Peripherals
//
//*****************************************************************************
static const uint32_t peripherals[] = {
    SYSCTL_PERIPH_GPIOA,
    SYSCTL_PERIPH_GPIOB,
    SYSCTL_PERIPH_GPIOC,
    SYSCTL_PERIPH_GPIOD,
    SYSCTL_PERIPH_GPIOE,
    SYSCTL_PERIPH_GPIOF,

    SYSCTL_PERIPH_ADC0, // ADC
    SYSCTL_PERIPH_ADC1, // ADC

    SYSCTL_PERIPH_TIMER0, // Tick
    SYSCTL_PERIPH_TIMER1, // ADCTimer

    SYSCTL_PERIPH_UDMA,
};

#define NUM_PERIPHERALS (sizeof(peripherals) / sizeof(uint32_t))


static inline void
ConfigurePeripherals(void)
{
    int i;
    bool ready = false;

    for (i = 0; i < NUM_PERIPHERALS; ++i) {
        SysCtlPeripheralEnable(peripherals[i]);
    }

    while (!ready) {
        ready = true;
        for (i = 0; i < NUM_PERIPHERALS; ++i) {
            ready &= SysCtlPeripheralReady(peripherals[i]);
        }
    }
}


//*****************************************************************************
//
// The error routine that is called if an ASSERT fails. (driverlib/debug.h)
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *filename, uint32_t line)
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
uDMAErrorIntHandler(void)
{
    uDMAErrorStatusClear();
    while (1) {
    }
}


static inline void
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
    uDMAControlBaseSet(ControlTable);
}


//*****************************************************************************
//
// This is the main application entry function.
//
//*****************************************************************************
int
main(void)
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
    // Initialize Modules
    //
    TickInit();
    ADCGroupInit();
    LoggerInit();
    OscilloscopeInit();
    USBDeviceInit();

    while (1) {
        // it handles the modules
        LoggerMain();
        OscilloscopeMain();

        // it handles the command
        CommandHandlerMain();

        // it sends the reply
        USBDeviceMain();

        // it sleeps until it receives a command
        SysCtlSleep();
    }
}
