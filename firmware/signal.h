/*
 * signal.h
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

#ifndef SIGNAL_H_
#define SIGNAL_H_


#include <stdbool.h>
#include <stdint.h>

#include "debug.h"


typedef struct Signal {
    unsigned char active;
} tSignal;


extern tSignal signal;


void SignalSetSine(uint32_t multiplier, uint32_t predivider, uint32_t divider, uint32_t amplitude, uint32_t second);

void SignalStart(void);

void SignalStop(void);

void SignalInit(void);


#endif /* SIGNAL_H_ */
