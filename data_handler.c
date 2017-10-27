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
    uint16_t *buffer;
    uint32_t i, b;

    if(!ADCQueueEmpty(&adc.adc_queue))
    {

        adc_event = ADCQueueRead(&adc.adc_queue);

        // copy
        b = 0;

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

        ADCQueueRelease(&adc.adc_queue);

    }
}


void
DataHandlerInit(void)
{
    DataQueueInit(&data_handler.data_queue);
}
