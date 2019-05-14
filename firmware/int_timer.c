/*
 * int_timer.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "int_timer.h"


void
Timer0AHandler(void)
{
    IntTimerIntHandler(&int_timer);
}
