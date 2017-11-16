
/*
 * main.c
 *
 *  Created on: 07.10.2017
 *      Author: Christoph Simon
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
#include "command_handler.h"
#include "reply_handler.h"
#include "data_handler.h"
#include "debug.h"
#include "usb_device.h"
#include "peripherals.h"
#include "adc.h"
#include "timer.h"
#include "logger.h"


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
    // Configure Timer
    //
    TimerInit();

    //
    // Initialize Command, Data and Reply Handler
    //
    CommandHandlerInit();
    DataHandlerInit();
    ReplyHandlerInit();

    LoggerInit();

    //
    // Print a string.
    //
    DEBUG_PRINT("Red Firmware\n");
    DEBUG_PRINT("Tiva C Series @ %u MHz\n", SysCtlClockGet() / 1000000);

    //ADCStart();

    while(1)
    {
        CommandHandlerMain();
        DataHandlerMain();
        ReplyHandlerMain();
        USBDeviceMain();
    }

}