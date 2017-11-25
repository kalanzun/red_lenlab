/*
 * adc_queue.h
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

#ifndef ADC_QUEUE_H_
#define ADC_QUEUE_H_


#include <stdbool.h>
#include <stdint.h>


#define ADC_QUEUE_LENGTH 8
#define ADC_PAYLOAD_LENGTH 1024


typedef struct ADCEvent {
    uint16_t payload[ADC_PAYLOAD_LENGTH];
    uint32_t length;
} tADCEvent;


typedef struct ADCQueue {
    tADCEvent queue[ADC_QUEUE_LENGTH];
    uint32_t read;
    uint32_t write;
    uint32_t acquire;
} tADCQueue;


inline bool
ADCQueueEmpty(tADCQueue *self)
{
    return self->read == self->write;
}


inline bool
ADCQueueFull(tADCQueue *self)
{
    return (self->acquire+1) % ADC_QUEUE_LENGTH == self->read;
}


inline tADCEvent*
ADCQueueAcquire(tADCQueue *self)
{
    tADCEvent *event = self->queue + self->acquire;
    self->acquire = (self->acquire + 1) % ADC_QUEUE_LENGTH;
    return event;
}


inline void
ADCQueueWrite(tADCQueue *self)
{
    self->write = (self->write + 1) % ADC_QUEUE_LENGTH;
}


inline tADCEvent*
ADCQueueRead(tADCQueue *self)
{
    return self->queue + self->read;
}


inline void
ADCQueueRelease(tADCQueue *self)
{
    self->read = (self->read + 1) % ADC_QUEUE_LENGTH;
}


inline void
ADCQueueInit(tADCQueue *self)
{
    self->read = 0;
    self->write = 0;
    self->acquire = 0;
}


#endif /* ADC_QUEUE_H_ */
