/*
 * timer.h
 *
 *  Created on: 11.11.2017
 *      Author: Christoph
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
