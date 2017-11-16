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
    ASSERT(logger.locked);

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

bool
LoggerAcquire(void)
{
    DEBUG_PRINT("acquire\n");

    if (logger.locked)
        return 0;

    if (!TimerAcquire())
        return 0;

    DEBUG_PRINT("ok\n");

    logger.locked = 1;
    return 1;
}

void
LoggerRelease(void)
{
    if (logger.locked) {
        TimerRelease();

        logger.locked = 0;
    }
}

void
LoggerInit(void)
{
    logger.locked = 0;
    logger.active = 0;
    logger.interval = 1000;
}
