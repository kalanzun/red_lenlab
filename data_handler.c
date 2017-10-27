/*
 * data_handler.c
 *
 *  Created on: 27.10.2017
 *      Author: Christoph
 */


#include <stdint.h>
#include "driverlib/debug.h"
#include "adc.h"
#include "data_handler.h"


tDataHandler data_handler;


void
DataHandlerMain(void)
{
    tADCEvent *adc_event;
    tDataEvent *data_event;
    uint8_t *buffer;
    uint32_t i, b;

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

    }
}


void
DataHandlerInit(void)
{
    DataQueueInit(&data_handler.data_queue);
}
