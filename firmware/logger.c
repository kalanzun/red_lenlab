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
#include "adc.h"
#include "driverlib/debug.h"
#include "timer.h"
#include "logger.h"
#include "reply_handler.h"
#include "debug.h"

#include "inc/hw_memmap.h"

tLogger logger;


void
LoggerStart(uint32_t interval)
{
    //DEBUG_PRINT("start\n");

    if (logger.active)
        return;

    logger.active = 1;
    logger.request_stop = false;

    logger.interval = interval;

    DEBUG_PRINT("LoggerStart %d\n", interval);

    ADCStartSingle(interval);
}

void
LoggerStop(void)
{
    DEBUG_PRINT("LoggerStop\n");

    if (logger.active) {
        logger.request_stop = true;
    }
}

void
LoggerMain(void)
{
    tEvent *reply;
    uint32_t array[2]; // TODO without itermediate copy?

    if (!logger.active)
        return;

    if (ADCSingle()) {
        DEBUG_PRINT("ADCSingle\n");
        ADCSingleGet(array, array+1);
        ADCSingleRelease();

        reply = QueueAcquire(&reply_handler.reply_queue);

        EventSetReply(reply, LoggerData);
        EventSetIntArray(reply, array, 2);

        if (logger.request_stop) {
            ADCStop();
            logger.active = 0;
        }
        else {
            EventSetLastPackage(reply, 0);
        }

        QueueWrite(&reply_handler.reply_queue);
    }
}

void
LoggerInit(void)
{
    logger.active = 0;
    logger.request_stop = 0;
    logger.interval = 1000;
}
