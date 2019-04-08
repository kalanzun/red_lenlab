/*
 * error.h
 *
 */

#ifndef ERROR_H_
#define ERROR_H_


typedef enum Error {
    OK,
    LOCK_ERROR,
    STATE_ERROR,
    MEMORY_ERROR,
    NUM_ERRORS
} tError;


#endif /* ERROR_H_ */
