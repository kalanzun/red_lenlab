
#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "com_device.h"


//*****************************************************************************
//
// The error routine that is called if an ASSERT fails. (driverlib/debug.h)
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
    while (1);
}
#endif


//*****************************************************************************
//
// This is the main application entry function.
//
//*****************************************************************************
int
main(void)
{
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    //MAP_FPULazyStackingEnable();

    //
    // System Clock 80 MHz
    //
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    //
    // Configure the required pins for USB operation.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    MAP_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);

    //
    // Initialize USB
    //
    COMDeviceInit();

    //
    // Main application loop.
    //
    while(1)
    {
        COMDeviceMain();
    }
}
