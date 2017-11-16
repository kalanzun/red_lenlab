/*
 * logger.c
 *
 *  Created on: 11.11.2017
 *      Author: Christoph
 */

#include <stdbool.h>
#include <stdint.h>
#include "driverlib/debug.h"
#include "timer.h"
#include "logger.h"
#include "debug.h"


tLogger logger;


void
LoggerStart(void)
{
    DEBUG_PRINT("start\n");

    if (logger.active)
        return;

    logger.active = 1;

    TimerSetInterval(logger.interval);
    TimerStart();
}

void
LoggerStop(void)
{
    DEBUG_PRINT("stop\n");
    if (logger.active) {
        TimerStop();
        logger.active = 0;
    }
}

void
LoggerSetInterval(uint32_t interval)
{
    DEBUG_PRINT("logger setInterval %d", interval);
    logger.interval = interval;
}

void
LoggerInit(void)
{
    logger.interval = 1000;
}
