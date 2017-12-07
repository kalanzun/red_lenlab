/*
 * data_handler.c
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
#include "driverlib/debug.h"
#include "adc.h"
#include "data_handler.h"
#include "reply_handler.h"
#include "timer.h"
#include "debug.h"


tDataHandler data_handler;


#if 0
void
DataHandlerMain(void)
{
    tADCEvent *adc_event;
    tDataEvent *data_event;
    uint8_t *buffer;
    uint32_t i; //, b;
    uint16_t current_value;
    int32_t current_delta;
    int8_t delta;

    if(!ADCQueueEmpty(&adc.adc_queue))
    {

        adc_event = ADCQueueRead(&adc.adc_queue);
#if 0
        // copy
        b = 0;

        ASSERT(0); // buffer is uint16_t here

        // We assume that ADC_PAYLOAD_LENGTH is a multiple of DATA_PAYLOAD_LENGTH and
        // ADC payload is uint16_t, but data payload is uint8_t.
        ASSERT(DATA_PAYLOAD_LENGTH % 2 == 0);
        ASSERT(ADC_PAYLOAD_LENGTH % (DATA_PAYLOAD_LENGTH / 2) == 0);

        // This copy function sends all the data of one ADC event to the data queue at once.
        // Data events are smaller, so the data queue needs to be long enough.

        for (i = 0; i < ADC_PAYLOAD_LENGTH; i++)
        {
            if (b == 0)
            {
                ASSERT(!DataQueueFull(&data_handler.data_queue));
                data_event = DataQueueAcquire(&data_handler.data_queue);
                buffer = (uint16_t *) data_event->payload;
            }
            buffer[b] = adc_event->payload[i];
            b = b + 1;
            if (b == DATA_PAYLOAD_LENGTH / 2)
            {
                DataQueueWrite(&data_handler.data_queue);
                b = 0;
            }
        }
#endif
#if 0
        // compress 2 uint16_t with 12 bit values into 3 uint8_t
        b = 0;

        // We assume that ADC_PAYLOAD_LENGTH is a multiple of 3*DATA_PAYLOAD_LENGTH and
        // ADC payload is uint16_t, but data payload is uint8_t.
        ASSERT(DATA_PAYLOAD_LENGTH == 1024);
        ASSERT((ADC_PAYLOAD_LENGTH / 2) % (1020 / 3) == 0);
        // 1020 bytes payload, 4 bytes header

        // This copy function sends all the data of one ADC event to the data queue at once.
        // Data events are smaller, so the data queue needs to be long enough.

        for (i = 0; i < (ADC_PAYLOAD_LENGTH / 2); i++)
        {
            if (b == 0)
            {
                ASSERT(!DataQueueFull(&data_handler.data_queue));
                data_event = DataQueueAcquire(&data_handler.data_queue);
                buffer = data_event->payload;
                buffer[b++] = 0x01; // adc data event
                buffer[b++] = 0x01; // compression algorithm 1
                buffer[b++] = 0x00; // not used
                buffer[b++] = 0x00; // not used
            }
            buffer[b++] = (uint8_t) adc_event->payload[i];
            buffer[b++] = ((uint8_t) (adc_event->payload[i] >> 8) & 0x0F) || ((uint8_t) (adc_event->payload[i+1] << 4) & 0xF0);
            buffer[b++] = (uint8_t) (adc_event->payload[i+1] >> 4);
            if (b == DATA_PAYLOAD_LENGTH)
            {
                DataQueueWrite(&data_handler.data_queue);
                b = 0;
            }
        }
        ADCQueueRelease(&adc.adc_queue);
#endif
#if 0
        // compress uint16_t with 12 bit values into uint8_t with deltas
        //b = 0;

        // ADC payload is uint16_t, but data payload is uint8_t.
        ASSERT(DATA_PAYLOAD_LENGTH == 1024);
        ASSERT(ADC_PAYLOAD_LENGTH == 1021);
        // 1020 bytes payload, 4 bytes header, header consumes first ADC item

        // This copy function sends all the data of one ADC event to the data queue at once.

        current_value = adc_event->payload[0];

        ASSERT(!DataQueueFull(&data_handler.data_queue));
        data_event = DataQueueAcquire(&data_handler.data_queue);
        buffer = data_event->payload;
        buffer[0] = 0x01; // adc data event
        buffer[1] = 0x02; // compression algorithm 2
        *((uint16_t *) buffer + 2) = current_value; // start value

        for (i = 1; i < ADC_PAYLOAD_LENGTH; i++)
        {
            current_delta = adc_event->payload[i] - current_value;
            // delta is equal to current_delta with saturation at -127 and 127
            if (current_delta < 0)
            {

                if (current_delta < -127) // this is not -128, the max. slope shall be identical for rising and falling edges
                    delta = -127;
                else
                    delta = 0x80 | (~(current_delta & 0x7F) + 1);
            }
            else
            {
                if (current_delta > 127)
                    delta = 127;
                else
                    delta = current_delta & 0x7F;
            }
            current_value += delta;
            buffer[3+i] = (uint8_t) delta;
        }
#endif

        DataQueueWrite(&data_handler.data_queue);
        ADCQueueRelease(&adc.adc_queue);

    }
}
#endif

/*
void
DataHandlerMain_for_logger(void)
{
    tADCEvent *adc_event;
    tEvent *reply;

    uint32_t *buffer;
    uint32_t i;

    if(!ADCQueueEmpty(&adc.adc_queue))
    {
        ASSERT(!QueueFull(&reply_handler.reply_queue));

        adc_event = ADCQueueRead(&adc.adc_queue);
        reply = QueueAcquire(&reply_handler.reply_queue);

        EventSetCommand(reply, startLogger);
        EventSetReply(reply, uInt32);
        *((uint16_t *) (reply->payload + 2)) = ADC_PAYLOAD_LENGTH / 4;

        *((uint32_t *) (reply->payload + 4)) = *((uint32_t *) (adc_event->payload));

        DEBUG_PRINT("reply %d\n", *((uint32_t *) (reply->payload + 4)));

        buffer = (uint32_t *) (reply->payload + 8);

        for (i = 0; i < 4; i++) buffer[i] = 0;

        for (i = 0; i < ADC_PAYLOAD_LENGTH - 4; i++)
        {
            buffer[i%4] += adc_event->payload[4+i];
        }

        reply->length = 4 + 4 + 4*4;

        QueueWrite(&reply_handler.reply_queue);
        ADCQueueRelease(&adc.adc_queue);
    }
}
*/

void
DataHandlerMain(void)
{
    tDataEvent *data_event;
    //tEvent *reply;

    uint8_t *buffer;
    uint32_t i;
    //static uint16_t a[] = {0, 0};
    static uint8_t count = 8;

    if(adc.ping_ready)
    {
        ASSERT(!DataQueueFull(&data_handler.data_queue));
        data_event = DataQueueAcquire(&data_handler.data_queue);

        data_event->payload[0] = startOscilloscope;
        data_event->payload[1] = Int8;
        data_event->payload[2] = 1; // encoding not compressed bytes starting at 4
        data_event->payload[3] = 0;

        buffer = data_event->payload + 4;

        for (i = 0; i < ADC_PAYLOAD_LENGTH; i++)
        {
            //buffer[i] = i>>2;
            buffer[i] = (adc.ping[i]) >> 4; //  - a[i%2]
            //a[i%2] = adc.ping[i];
        }

        data_event->length = 4 + ADC_PAYLOAD_LENGTH;

        DataQueueWrite(&data_handler.data_queue);
        adc.ping_ready = 0;

        count--;
        if (count == 0) {
            //DEBUG_PRINT("data handler done\n");
            ADCStop();

            data_event = DataQueueAcquire(&data_handler.data_queue);
            data_event->payload[0] = stopOscilloscope;
            DataQueueWrite(&data_handler.data_queue);
        }
    }
    if(adc.pong_ready)
    {
        ASSERT(!DataQueueFull(&data_handler.data_queue));
        data_event = DataQueueAcquire(&data_handler.data_queue);

        data_event->payload[0] = startOscilloscope;
        data_event->payload[1] = Int8;
        data_event->payload[2] = 1; // encoding not compressed bytes starting at 4
        data_event->payload[3] = 0;

        buffer = data_event->payload + 4;

        for (i = 0; i < ADC_PAYLOAD_LENGTH; i++)
        {
            //buffer[i] = i>>2;
            buffer[i] = (adc.pong[i]) >> 4; //  - a[i%2]
            //a[i%2] = adc.ping[i];
            //buffer[i] = (adc.pong[i] - a[i%2]) >> 2;
            //a[i%2] = adc.pong[i];
        }

        data_event->length = 4 + ADC_PAYLOAD_LENGTH;

        DataQueueWrite(&data_handler.data_queue);
        adc.pong_ready = 0;

        count--;
        if (count == 0) {
            //DEBUG_PRINT("data handler done\n");
            ADCStop();

            data_event = DataQueueAcquire(&data_handler.data_queue);
            data_event->payload[0] = stopOscilloscope;
            DataQueueWrite(&data_handler.data_queue);
        }
    }
}

void
DataHandlerInit(void)
{
    DataQueueInit(&data_handler.data_queue);
}
