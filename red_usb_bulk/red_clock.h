/*
 * red_clock.h
 *
 *  Created on: 24.11.2021
 *      Author: kalan
 */

#ifndef RED_CLOCK_H_
#define RED_CLOCK_H_


struct RedClock {
    uint32_t cycles_per_ms;
    uint32_t cycles_per_us;
};


// Avoid name collision. There is a standard clock() function.
extern struct RedClock red_clock;


inline void
RedClockInit(void)
{
    red_clock.cycles_per_ms = SysCtlClockGet() / 1000;
    red_clock.cycles_per_us = red_clock.cycles_per_ms / 1000;

    //
    // Enable systick for timing
    //
    // allowed max value is 16,777,216
    //SysTickPeriodSet(100 * red_clock.cycles_per_ms);
    //SysTickEnable();

}


#endif /* RED_CLOCK_H_ */
