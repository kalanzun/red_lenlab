/*
 * logger.h
 *
 *  Created on: 11.11.2017
 *      Author: Christoph
 */

#ifndef LOGGER_H_
#define LOGGER_H_


typedef struct Logger {
    bool locked;
    bool active;
    uint32_t interval; // milliseconds
} tLogger;


extern tLogger logger;


void LoggerStart(void);
void LoggerStop(void);

void LoggerSetInterval(uint32_t interval);

bool LoggerAcquire(void);
void LoggerRelease(void);

void LoggerInit(void);


#endif /* LOGGER_H_ */
