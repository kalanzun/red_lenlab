/*
 * timer.h
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

#ifndef TIMER_H_
#define TIMER_H_


typedef struct Timer {
    bool active;
    volatile uint32_t interval;
    volatile uint32_t time;
} tTimer;


extern tTimer timer;


void TimerStart(void);
void TimerStop(void);

void TimerSetInterval(uint32_t interval);

void TimerInit(void);


#endif /* TIMER_H_ */
