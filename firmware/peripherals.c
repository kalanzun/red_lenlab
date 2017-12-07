/*
 * peripherals.c
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
#include "driverlib/sysctl.h"


const uint32_t peripherals[] = {
    SYSCTL_PERIPH_GPIOA,
    SYSCTL_PERIPH_GPIOB,
    SYSCTL_PERIPH_GPIOC,
    SYSCTL_PERIPH_GPIOD,
    SYSCTL_PERIPH_GPIOE,
    SYSCTL_PERIPH_GPIOF,

    SYSCTL_PERIPH_ADC0,
    SYSCTL_PERIPH_ADC1,

    SYSCTL_PERIPH_TIMER0,

    SYSCTL_PERIPH_SSI0,

    SYSCTL_PERIPH_UDMA
};

#define NUM_PERIPHERALS (sizeof(peripherals) / sizeof(uint32_t))


inline void
ConfigurePins(void)
{
    uint8_t i;

    for (i=0; i<NUM_PERIPHERALS; i++)
        SysCtlPeripheralEnable(peripherals[i]);

    for (i=0; i<NUM_PERIPHERALS; i++)
        while(!SysCtlPeripheralReady(peripherals[i]))
        {
        }
}
