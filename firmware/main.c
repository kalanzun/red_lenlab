
/*
 * main.c
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

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/udma.h"
#include "driverlib/systick.h"
#include "command_handler.h"
#include "reply_handler.h"
#include "debug.h"
#include "usb_device.h"
#include "peripherals.h"
#include "adc.h"
#include "timer.h"
#include "logger.h"
#include "oscilloscope.h"
#include "ssi.h"
#include "signal.h"
#include "state_machine.h"


//*****************************************************************************
//
// The control table used by the uDMA controller.  This table must be aligned
// to a 1024 byte boundary.
//
//*****************************************************************************
#if defined(ewarm)
#pragma data_alignment=1024
uint8_t ui8ControlTable[1024];
#elif defined(ccs)
#pragma DATA_ALIGN(ui8ControlTable, 1024)
uint8_t ui8ControlTable[1024];
#else
uint8_t ui8ControlTable[1024] __attribute__ ((aligned(1024)));
#endif


//*****************************************************************************
//
// The error routine that is called if an ASSERT fails. (driverlib/debug.h)
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    UARTprintf("Error at line %d of %s\n", ui32Line, pcFilename);
    while(1)
    {
    }
}
#endif


//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
inline void
ConfigureUART(void)
{
#ifdef DEBUG
    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 80000000);
#endif
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
int
main(void) {
    //
    // System Clock 80 MHz
    //
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    //
    // Enable Systick for timing
    //
    SysTickPeriodSet(16777216);
    SysTickEnable();

    //
    // Initialize State Machine
    //
    StateMachineInit(&state_machine);

    //
    // GPIO Pin Configuration
    //
    ConfigurePins();

    //
    // Configure UART for DEBUG_PRINT
    //
    ConfigureUART();

    //
    // Configure uDMA
    //
    ConfigureuDMA();

    //
    // Configure USB Device
    //
    USBDeviceInit();

    //
    // Configure ADC
    //
    ADCInit();

    //
    // Configure SSI
    //
    SSIInit();

    //
    // Configure Timer
    //
    TimerInit();

    //
    // Initialize Command, Data and Reply Handler
    //
    CommandHandlerInit();
    ReplyHandlerInit();
    SignalInit();
    LoggerInit();
    OscilloscopeInit(&oscilloscope);

    //
    // Print a string.
    //
    DEBUG_PRINT("Red Firmware\n");
    DEBUG_PRINT("Tiva C Series @ %u MHz\n", SysCtlClockGet() / 1000000);

    while(1)
    {
        CommandHandlerMain();
        LoggerMain();
        OscilloscopeMain(&oscilloscope);
        ReplyHandlerMain();
        USBDeviceMain();
    }

}
