/*
 * microtest.h
 *
 */

#ifndef MICROTEST_H_
#define MICROTEST_H_


#include "driverlib/debug.h"

#include "debug.h"
#include "error.h"


#define test() DEBUG_PRINT("TEST %s (%s:%i)", __FUNCTION__, __FILE__, __LINE__)


#define fail(msg, ...) DEBUG_PRINT("FAIL " msg, ##__VA_ARGS__)


#define assert(expr) do { if (!(expr)) { fail(#expr); ASSERT(0); } } while (0)


#define ok() DEBUG_PRINT("OK")


#endif /* MICROTEST_H_ */
