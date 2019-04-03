/*
 * wait.c
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "wait_timer.h"


tWaitTimer wait_timer;


void
Timer0AIntHandler(void)
{
    WaitTimerIntHandler(&wait_timer);
}


void
wait(uint32_t interval)
{
    WaitTimerWait(&wait_timer, interval);
}


void
ConfigureWait(void)
{
    WaitTimerInit(&wait_timer);
}
