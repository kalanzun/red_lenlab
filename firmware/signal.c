/*
 * signal.c
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
#include "signal.h"
#include "lenlab_protocol.h"
#include "debug.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "ssi.h"
#include "reply_handler.h"

tSignal signal;

inline int32_t
fixed(int32_t value)
{
    return value << 11;
}

inline int32_t
f_div(int32_t a, int32_t b)
{
    return (a << 11) / b;
}

inline int32_t
f_mul(int32_t a, int32_t b)
{
    return (a * b) >> 11;
}

// in fixed point 11 bit
#define f_PI (6434)
#define f_PI2 (3217)

inline int32_t
taylor(int32_t x)
{
    int32_t x2 = f_mul(x, x);
    int32_t x3 = f_mul(x2, x);
    int32_t x5 = f_mul(x3, x2);
    int32_t x7 = f_mul(x5, x2);

    return (x) - (x3 / 6) + (x5 / 120) - (x7 / 5040);
}

void
SignalCalculateSine(void)
{
    /*
    // 1.2 ms mit 4 mal taylor
    // 0.5 ms mit 1 mal taylor und kopieren
    uint32_t i;
    tDataEvent *event = DataQueueAcquire(&data_handler.data_queue);
    int16_t *buffer = (int16_t *) (event->payload + 8);
    volatile uint32_t start = SysTickValueGet();
    volatile uint32_t time;
    DEBUG_PRINT("start %u", start);
    // fixed point arithmetic (2^11)F = 1
    // -> pi/2 = (3217)F
    // about 512 steps for one period, 128 for pi/2, (26)F per step
    // so 124 steps for pi/2
    for (i = 0; i < 124; i++)
        buffer[i] = taylor(i*26);

    for (i = 0; i < 124; i++)
        buffer[124+i] = taylor((124-i)*26);
    for (i = 0; i < 124; i++)
        buffer[248+i] = -taylor(i*26);
    for (i = 0; i < 124; i++)
        buffer[248+124+i] = -taylor((124-i)*26);

    for (i = 0; i < 124; i++)
        buffer[124+i] = buffer[123-i];
    for (i = 0; i < 124; i++)
        buffer[248+i] = -buffer[i];
    for (i = 0; i < 124; i++)
        buffer[248+124+i] = -buffer[123-i];

    event->payload[0] = calculateSine;
    event->payload[1] = Int16;
    event->length = 4+4+2*495;
    time = SysTickValueGet();
    *((uint32_t *) (event->payload + 4)) = start - time; // der timer zählt runter!
    DEBUG_PRINT("done %u", time);
    DataQueueWrite(&data_handler.data_queue);
    */
}

#define SIGNAL_OFFSET 2048
#define SIGNAL_MIN 0
#define SIGNAL_MAX 4095

inline uint16_t
SignalDACFormat(int32_t value, bool channel)
{
    value += SIGNAL_OFFSET;
    value = value > 0 ? value : 0;
    value = value < SIGNAL_MAX ? value : SIGNAL_MAX;
    value |= channel ? 0x3000 : 0xB000;
    return (uint16_t) value;
}

void
SignalWriteSine400(uint16_t *buffer)
{
    int32_t value;
    int32_t i;

    for (i = 0; i < 200; i++)
    {
        //value = taylor(f_div(f_mul(fixed(i), f_PI), length / 2) - f_PI2);
        value = taylor(i * f_PI / 200 - f_PI2);

        //value = taylor((32942 * i) >> 10);

        buffer[2*(      i)  ] = SignalDACFormat(value, 0);
        buffer[2*(400-1-i)  ] = SignalDACFormat(value, 0);

        buffer[2*(      i)+1] = SignalDACFormat(value, 1);
        buffer[2*(400-1-i)+1] = SignalDACFormat(value, 1);

        //buffer[2*(100-1-i)+1] = SignalDACFormat(-value, 1);
        //buffer[2*(100  +i)+1] = SignalDACFormat( value, 1);
        //buffer[2*(300-1-i)+1] = SignalDACFormat( value, 1);
        //buffer[2*(300  +i)+1] = SignalDACFormat(-value, 1);
    }
}


void
SignalWriteSine(uint16_t *buffer, uint32_t length, uint32_t interleaved, uint32_t channel)
{
    int32_t value;
    int32_t i;
    int32_t index;

    // if interleaved is true, buffer needs twice the length

    for (i = 0; i < length / 2; i++)
    {
        value = taylor(2 * i * f_PI / length - f_PI2);

        //i = interleaved ? 2*x + channel : x;
        index = 2*i + channel;
        buffer[index] = SignalDACFormat(value, channel);

        //i = interleaved ? 2*(length-1 - x) + channel : (length-1 - x);
        index = 2*(length-1 - i) + channel;
        buffer[index] = SignalDACFormat(value, channel);
    }
}
#define SIGNAL_BASE_FREQUENCY (2500)

void
SignalUpdate(uint16_t frequency)
{
    uint16_t base, c, d;
    uint16_t a, b;
    uint32_t delta;

    for (b = 1 + ((frequency-1) / SIGNAL_BASE_FREQUENCY);
            b <= 20;
            b++)
    {
        base = SIGNAL_BASE_FREQUENCY * b;
        a = base / frequency; // abgerundet
        c = (base / a) - frequency; // aufgerundet
        d = frequency - (base / (a+1)); // abgerundet
        // ist c oder d näher?
        if (d < c) // d
        {
            delta = (d << 16) / frequency;
            a += 1;
        }
        else // c
        {
            delta = (c << 16) / frequency;
        }
        // ist delta kleiner als 10%?
        if (delta < 6554)
        {
            signal.frequency_divisor = a;
            signal.memory_multiplier = b;
            return;
        }
    }
    ASSERT(0);
}

#define max(a, b) ((a) > (b) ? (a) : (b))

void
SignalTestSineFrequency(tEvent *cmd)
{
    uint16_t base, c, d;
    tEvent *reply;
    uint16_t frequency;
    uint16_t a, b;
    uint32_t delta;
    uint32_t signal_base_frequency;

    signal_base_frequency = SysCtlClockGet() / 16 / 500 / 2;

    reply = QueueAcquire(&reply_handler.reply_queue);
    frequency = *((uint16_t *) (cmd->payload + 4));

    ASSERT(cmd->length == 6);

    for (b = 1 + ((frequency-1) / (4*signal_base_frequency));
            b <= 20;
            b++)
    {
        base = signal_base_frequency * b;
        a = max(base / frequency, 4); // abgerundet
        c = (base / a) - frequency; // aufgerundet
        d = frequency - (base / (a+1)); // abgerundet
        // ist c oder d näher?
        if (d < c) // d
        {
            delta = (d << 16) / frequency;
            a += 1;
        }
        else // c
        {
            delta = (c << 16) / frequency;
        }
        // ist delta kleiner als 3%?
        if (delta < 1966)
        {
            DEBUG_PRINT("f = %u; a = %u; b = %u;\n", frequency, a, b);
            *(uint32_t *) (reply->payload + 4) = a;
            *(uint32_t *) (reply->payload + 8) = b;
            reply->length = 12;
            QueueWrite(&reply_handler.reply_queue);
            return;
        }
    }

    // send error code 0 0
    *(uint32_t *) (reply->payload + 4) = 0;
    *(uint32_t *) (reply->payload + 8) = 0;
    reply->length = 12;
    QueueWrite(&reply_handler.reply_queue);

}


void
SignalSetFrequency(uint16_t frequency, uint16_t divisor)
{
    uint32_t delta;
    uint16_t *buffer = SSIGetBuffer();
    uint16_t i;
    uint16_t a;
    uint16_t b;

    SignalUpdate(frequency);

    a = signal.frequency_divisor;
    b = signal.memory_multiplier;

    DEBUG_PRINT("a = %u; b = %u;\n", a, b);

    //delta = (SSI_BUFFER_LENGTH << 16) / 2 / b;
    delta = SSI_BUFFER_LENGTH / 2 / b;

    for (i = 0; i < b; i++)
    {
        SignalWriteSine(buffer + 2*i*delta, delta, 1, 0);
        SignalWriteSine(buffer + 2*i*delta, delta, 1, 1);
    }

}


void
SignalStart(void)
{

    //uint16_t *buffer;

    //buffer = SSIGetBuffer();
    //SignalWriteSine400(buffer);
    //SignalWriteSine(buffer, 500, 1, 0); SignalWriteSine(buffer, 500, 1, 1);
    SSISetLength(1000);

    SignalSetFrequency(128, 1);

    //SSISetFrequency(2 * SysCtlClockGet() / signal.frequency_divisor);
    SSISetFrequency(2500000 / signal.frequency_divisor);
    SSIStart();
}

void
SignalInit(void)
{

}
