
/*
 * main.c
 *
 *  Created on: 07.10.2017
 *      Author: Christoph Simon
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


//*****************************************************************************
//
// Debug-related definitions and declarations.
//
// Debug output is available via UART0 if DEBUG is defined during build.
//
//*****************************************************************************
#ifdef DEBUG
// Map all debug print calls to UARTprintf in debug builds.
#define DEBUG_PRINT UARTprintf

#else

// Compile out all debug print calls in release builds.
#define DEBUG_PRINT while(0) ((int (*)(char *, ...))0)
#endif


//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
#ifdef DEBUG
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }

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
    // Configure UART for DEBUG_PRINT
    //
    ConfigureUART();

    //
    // Print a string.
    //
    DEBUG_PRINT("Red Firmware Init!\n");

    while(1)
    {
    }

}
