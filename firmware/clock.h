/*
 * clock.h
 *
 */

#ifndef CLOCK_H_
#define CLOCK_H_


typedef struct Clock {
    uint32_t cycles_per_ms;
    uint32_t cycles_per_us;
} tClock;


extern tClock clock;


inline void ClockInit(tClock *self);


#endif /* CLOCK_H_ */
