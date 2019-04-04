/*
 * microtest.h
 *
 */

#ifndef MICROTEST_H_
#define MICROTEST_H_


#include "debug.h"


#define test() do { DEBUG_PRINT("TEST %s (%s:%i)", __FUNCTION__, __FILE__, __LINE__); } while (0)

#define fail(msg, ...) do { DEBUG_PRINT("FAIL " msg, ##__VA_ARGS__); return; } while (0)

#define assert(exp) do { if (!(exp)) { fail(#exp); } } while (0)

// when failing, these macros evaluate a and b a second time for the message

#define assertIntEqual(a, b) do { if (!((a) == (b))) { fail(#a " == " #b ", (" #a ": %i, " #b ": %i)", (a), (b)); } } while (0)

#define assertIntGreater(a, b) do { if (!((a) > (b))) { fail(#a " > " #b ", (" #a ": %i, " #b ": %i)", (a), (b)); } } while (0)

#define assertIntSmaller(a, b) do { if (!((a) < (b))) { fail(#a " < " #b ", (" #a ": %i, " #b ": %i)", (a), (b)); } } while (0)

#define ok() do { DEBUG_PRINT("OK"); } while (0)


#endif /* MICROTEST_H_ */
