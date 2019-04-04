/*
 * int_timer.c
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "int_timer.h"


void
Timer0AHandler(void)
{
    IntTimerIntHandler(&int_timer);
}
