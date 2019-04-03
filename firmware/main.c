/*
 * main.c
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "debug.h"
#include "utils/uartstdio.h"
#include "peripherals.h"

//*****************************************************************************
//
// The error routine that is called if an ASSERT fails. (driverlib/debug.h)
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
    UARTprintf("Error at line %d of %s\n", ui32Line, pcFilename);
    while (1)
    {
    }
}
#endif

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
inline void ConfigureUART(void)
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
    SysCtlClockSet(
    SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    //
    // Enable Systick for timing
    //
    SysTickPeriodSet(16777216);
    SysTickEnable();

    //
    // GPIO Pin Configuration
    //
    ConfigurePeripherals();

    //
    // Configure UART for DEBUG_PRINT
    //
    ConfigureUART();

    //
    // Print a welcome message
    //
    DEBUG_PRINT("Red Firmware TDD");
    DEBUG_PRINT("Tiva C Series @ %u MHz", SysCtlClockGet() / 1000000);

    while (1)
    {
    };
}
