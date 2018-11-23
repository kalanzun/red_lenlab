/*
 * oscilloscope.c
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

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/debug.h"
#include "oscilloscope.h"
#include "debug.h"
#include "adc.h"
#include "lenlab_protocol.h"
#include "memory.h"
#include "usb_device.h"
#include "state_machine.h"


tOscilloscope oscilloscope;


uint32_t
OscilloscopeStart(uint32_t samplerate)
{
    uint32_t error;

    if (!StateMachineGetState(&state_machine) == READY) return 1;
    if (oscilloscope.active) return 1;

    oscilloscope.active = 1;
    oscilloscope.trigger = 0;
    oscilloscope.count = 0;

    MemoryInit(&memory);
    error = ADCStart(8, samplerate);
    return error;
}


uint32_t
OscilloscopeStartTrigger(uint32_t samplerate)
{
    uint32_t i;

    if (!StateMachineGetState(&state_machine) == READY) return 1;
    if (oscilloscope.active) return 1;

    oscilloscope.active = 1;
    oscilloscope.trigger = 1;
    oscilloscope.count = 0;

    oscilloscope.filter_state = 0;
    oscilloscope.filter_index = 0;
    for (i = 0; i < OSCILLOSCOPE_FILTER_LENGTH; i++) oscilloscope.filter[i] = 0;

    oscilloscope.trigger_wait = 0;
    oscilloscope.trigger_active = 0;
    oscilloscope.trigger_save = 0;
    oscilloscope.trigger_post_count = 0;

    MemoryInit(&memory);
    RingAllocate(&oscilloscope.ring, 18);
    return ADCStart(2, samplerate); // Allocates 4
}


inline int8_t
delta(uint16_t previous, uint16_t next)
{
    int16_t delta = next - previous;
    if (delta > 127)
        return 127;
    else if (delta < -127)
        return -127;
    else
        return (int8_t) delta;
}


void
OscilloscopeMain()
{
    uint32_t i;
    tRing *ring0;
    tRing *ring1;
    tPage *page;
/*
    tPage *adc_page0;
    tPage *adc_page1;
    uint16_t *buffer0;
    uint16_t *buffer1;

    uint16_t state0, state1;
    int8_t *data;
*/
    if (!oscilloscope.active)
        return;

    if (!oscilloscope.trigger)
    {
        if (ADCReady()) // single done
        {
            //DEBUG_PRINT("OscilloscopeReady\n");

            ring0 = ADCGetRing(0);
            ring1 = ADCGetRing(1);

            //DEBUG_PRINT("%d, %d\n", ring0->read, ring1->read);

            for (i = 0; i < ring0->length; i++)
            {
                page = RingRead(ring0);

                page->buffer[0] = OscilloscopeData; // reply
                page->buffer[1] = ShortArray; // type
                page->buffer[2] = 0; // channel
                page->buffer[3] = 0; // last
                page->buffer[4] = i;
            }

            for (i = 0; i < ring1->length; i++)
            {
                page = RingRead(ring1);

                page->buffer[0] = OscilloscopeData; // reply
                page->buffer[1] = ShortArray; // type
                page->buffer[2] = 1; // channel
                page->buffer[3] = 0; // last
                page->buffer[4] = 10+i;
            }
            page->buffer[3] = 255;

            //DEBUG_PRINT("%d, %d\n", ring0->read, ring1->read);

            USBDeviceSendInterleaved(ring0, ring1);
            oscilloscope.active = 0;
        }
    }
    /*
    else
    {
        ring0 = ADCGetRing(0);
        ring1 = ADCGetRing(1);

        if (RingContent(ring0) && RingContent(ring1))
        {
            adc_page0 = RingRead(ring0);
            adc_page1 = RingRead(ring1);

            buffer0 = (uint16_t *) (adc_page0->buffer + 24);
            buffer1 = (uint16_t *) (adc_page1->buffer + 24);

            if (RingFull(&oscilloscope.ring)) {
                RingRead(&oscilloscope.ring);
                RingRelease(&oscilloscope.ring);
            }

            page = RingAcquire(&oscilloscope.ring);

            page->buffer[0] = OscilloscopeData;
            page->buffer[1] = ByteArray;
            page->buffer[2] = 0;
            page->buffer[3] = 0;

            state0 = buffer0[0] >> 2;
            state1 = buffer1[0] >> 2;
            *(uint16_t *) (page->buffer + 4) = state0;
            *(uint16_t *) (page->buffer + 6) = state1;
            *(uint16_t *) (page->buffer + 8) = 0; // trigger value

            data = (int8_t *) (page->buffer + OSCILLOSCOPE_HEADER_LENGTH);

            if (oscilloscope.count == 8) {
                oscilloscope.trigger_wait = 1;
            }

            for (i = 1; i < ADC_SAMPLES; i++)
            {
                data[2*i] = delta(state0, buffer0[i] >> 2);
                state0 += data[2*i];
                data[2*i+1] = delta(state1, buffer1[i] >> 2);
                state1 += data[2*i+1];

                if (oscilloscope.count == 7 || oscilloscope.trigger_wait || oscilloscope.trigger_active) {
                    oscilloscope.filter_state -= oscilloscope.filter[oscilloscope.filter_index];
                    oscilloscope.filter_state += state0;
                    oscilloscope.filter[oscilloscope.filter_index] = state0;
                    oscilloscope.filter_index = (oscilloscope.filter_index + 1) % OSCILLOSCOPE_FILTER_LENGTH;
                }

                // pre trigger, wait for signal to be below the trigger level
                if (oscilloscope.trigger_wait
                        && (oscilloscope.filter_state < (8*512))
                        ) {
                    oscilloscope.trigger_wait = 0;
                    oscilloscope.trigger_active = 1;
                }

                // trigger, wait for signal to cross the trigger level
                if (oscilloscope.trigger_active
                        && (oscilloscope.filter_state > (8*512))
                        ) {
                    oscilloscope.trigger_active = 0;
                    oscilloscope.trigger_save = 1;
                    *(uint16_t *) (page->buffer + 8) = i;
                }

                // trigger timeout after 5 full cycles of the ring buffer
                if ((oscilloscope.trigger_wait || oscilloscope.trigger_active) && oscilloscope.count == 5*18) {
                    oscilloscope.trigger_wait = 0;
                    oscilloscope.trigger_active = 0;
                    oscilloscope.trigger_save = 1;
                }

            }

            RingRelease(ring0);
            RingRelease(ring1);

            oscilloscope.count++;

            if (oscilloscope.trigger_save) {
                oscilloscope.trigger_post_count++;
                if (oscilloscope.trigger_post_count == 10) {
                    //DEBUG_PRINT("%d\n", oscilloscope.count);
                    oscilloscope.trigger_save = 0;
                    page->buffer[3] = 255; // mark this the last package
                    USBDeviceSend(&oscilloscope.ring);
                    oscilloscope.active = 0;
                }
            }
        }
    }
    */
}


void
OscilloscopeInit()
{
    oscilloscope.active = 0;
}
