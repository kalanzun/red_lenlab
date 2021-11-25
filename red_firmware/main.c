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
#include "clock.h"
#include "int_timer.h"
#include "logger.h"
#include "ring.h"
#include "signal.h"
#include "ssi.h"
#include "oscilloscope.h"
#include "trigger.h"
#include "command_handler.h"
#include "reply_handler.h"
#include "usb_device.h"


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
// Memory
//
//*****************************************************************************
tPage pages[MEMORY_LENGTH];
uint8_t reply_buffer[EVENT_QUEUE_BUFFER_LENGTH];
uint8_t shared_buffer[EVENT_QUEUE_BUFFER_LENGTH];
uint8_t command_buffer[EVENT_QUEUE_BUFFER_LENGTH];
uint16_t ssi_buffer[SSI_BUFFER_LENGTH];


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

    SYSCTL_PERIPH_SSI0,

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
tClock red_clock;
tIntTimer int_timer;
tMemory memory;
tADCGroup adc_group;
tLogger logger;
tOscSeqGroup osc_seq_group;
tOscilloscope oscilloscope;
tTrigger trigger;
tSSI ssi;
tSignal signal;
tCommandHandler command_handler;
tReplyHandler reply_handler;
tUSBDevice usb_device;


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
    // clock module
    //
    ClockInit(&red_clock);

    //
    // Configure peripherals
    //
    ConfigurePeripherals();

    //
    // Configure UART for DEBUG_PRINT
    //
#ifdef DEBUG
    ConfigureUART();
#endif

    //
    // Print a welcome message
    //
    DEBUG_PRINT("Red Firmware TDD");
    DEBUG_PRINT("Tiva C Series @ %u MHz", red_clock.cycles_per_us);

    //
    // Configure uDMA
    //
    ConfigureuDMA();

    //
    // int_timer module
    //
    IntTimerInit(&int_timer);

    //
    // memory module
    //
    MemoryInit(&memory, pages);

    //
    // adc_group module
    //
    ADCGroupInit(&adc_group);

    //
    // logger module
    //
    LoggerInit(&logger);

    //
    // osc_seq_group module
    //
    OscSeqGroupInit(&osc_seq_group);

    //
    // oscilloscope module
    //
    OscilloscopeInit(&oscilloscope);

    //
    // trigger module
    //
    TriggerInit(&trigger);

    //
    // ssi module
    //
    SSIInit(&ssi, ssi_buffer);

    //
    // signal module
    //
    SignalInit(&signal);

    //
    // usb_device module
    //
    USBDeviceInit(&usb_device);

    //
    // command handler module
    //
    CommandHandlerInit(&command_handler, command_buffer);

    //
    // reply handler module
    //
    ReplyHandlerInit(&reply_handler, reply_buffer, shared_buffer);

    //
    // Run tests
    //
#ifdef DEBUG
    tests();
#endif

    while (1) {
        CommandHandlerMain(&command_handler);
        LoggerMain(&logger);
        OscilloscopeMain(&oscilloscope, true);
        TriggerMain(&trigger, true);
        USBDeviceMain(&usb_device);
    }
}
