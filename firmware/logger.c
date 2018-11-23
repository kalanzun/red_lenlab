/*
 * logger.c
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

#include "driverlib/debug.h"

#include "adc.h"
#include "debug.h"
#include "reply_handler.h"
#include "state_machine.h"

#include "logger.h"


tLogger logger;


uint32_t
LoggerStart(uint32_t interval)
{
    uint32_t error;

    error = ADCLoggerStart(interval);

    if (!error) {
        StateMachineSetState(&state_machine, LOGGER);
    }

    return error;
}


void
LoggerStop()
{
    ADCLoggerStop();
    StateMachineSetState(&state_machine, READY);
}


void
LoggerMain()
{
    tEvent *reply;
    uint32_t array[2]; // TODO without copy?

    if (!StateMachineGetState(&state_machine) == LOGGER)
        return;

    if (ADCLoggerReady()) {
        ADCLoggerGet(array, array+1);
        ADCLoggerRelease();

        reply = QueueAcquire(&reply_handler.reply_queue);

        EventSetReply(reply, LoggerData);
        EventSetIntArray(reply, array, 2);

        QueueWrite(&reply_handler.reply_queue);
    }
}


void
LoggerInit()
{
}
