/*
 * peripherals.c
 *
 *  Created on: 23.10.2017
 *      Author: Christoph
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
