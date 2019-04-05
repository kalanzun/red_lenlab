/*
 * main.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "tests/tests.h"
#include "adc.h"
#include "debug.h"
#include "int_timer.h"
#include "logger.h"
#include "memory.h"
#include "oscilloscope.h"


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

    SYSCTL_PERIPH_TIMER0, // int_timer
    SYSCTL_PERIPH_TIMER1, // adc_timer

    SYSCTL_PERIPH_ADC0,
    SYSCTL_PERIPH_ADC1,

    SYSCTL_PERIPH_UDMA,
};

#define NUM_PERIPHERALS (sizeof(peripherals) / sizeof(uint32_t))


inline void
ConfigurePeripherals(void)
{
    uint8_t i;

    for (i=0; i<NUM_PERIPHERALS; i++)
        SysCtlPeripheralEnable(peripherals[i]);

    for (i=0; i<NUM_PERIPHERALS; i++)
        while(!SysCtlPeripheralReady(peripherals[i]))
        {
        }
}


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
// uDMA
//
//*****************************************************************************
void
uDMAErrorHandler(void)
{
    //uint32_t status;
    //status = uDMAErrorStatusGet();
    DEBUG_PRINT("uDMA Error\n");
    uDMAErrorStatusClear();
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
// Modules
//
//*****************************************************************************
tPage memory[MEMORY_LENGTH];

tADCGroup adc_group;
tIntTimer int_timer;
tLogger logger;
tOscilloscope oscilloscope;


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
    // Enable systick for timing
    //
    SysTickPeriodSet(SysCtlClockGet() / 10); // 100ms (the register is only 32 bits)
    SysTickEnable();

    //
    // Configure peripherals
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

    //
    // Configure uDMA
    //
    ConfigureuDMA();

    //
    // Module int_timer
    //
    IntTimerInit(&int_timer);

    //
    // Module adc_group
    //
    ADCGroupInit(&adc_group);

    //
    // Module logger
    //
    LoggerInit(&logger, &adc_group);

    //
    // Module oscilloscope
    //
    OscilloscopeInit(&oscilloscope, &adc_group);

    //
    // Run tests
    //
    tests();

    while (1)
    {
    };
}
