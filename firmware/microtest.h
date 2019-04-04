/*
 * microtest.h
 *
 */

#ifndef MICROTEST_H_
#define MICROTEST_H_


#include "debug.h"
#include "error.h"


#define test() DEBUG_PRINT("TEST %s (%s:%i)", __FUNCTION__, __FILE__, __LINE__)

#define fail(msg, ...) do { DEBUG_PRINT("FAIL " msg, ##__VA_ARGS__); } while (0)

#define assert(exp) do { if (!(exp)) { fail(#exp); return; } } while (0)

#define ok() DEBUG_PRINT("OK")


#define assertIntEqual(a, b) do { if (__assertIntEqual__(a, b, #a, #b)) return; } while (0)

inline bool
__assertIntEqual__(int a, int b, const char *_a, const char *_b)
{
    if (!(a == b)) {
        fail("%s == %s, {%s: %i, %s: %i}", _a, _b, _a, a, _b, b);
        return true;
    }
    return false;
}


#define assertIntGreater(a, b) do { if (__assertIntGreater__(a, b, #a, #b)) return; } while (0)

inline bool
__assertIntGreater__(int a, int b, const char *_a, const char *_b)
{
    if (!(a > b)) {
        fail("%s > %s, {%s: %i, %s: %i}", _a, _b, _a, a, _b, b);
        return true;
    }
    return false;
}


#define assertIntLess(a, b) do { if (__assertIntLess__(a, b, #a, #b)) return; } while (0)

inline bool
__assertIntLess__(int a, int b, const char *_a, const char *_b)
{
    if (!(a < b)) {
        fail("%s < %s, {%s: %i, %s: %i}", _a, _b, _a, a, _b, b);
        return true;
    }
    return false;
}


#define assertTrue(exp) do { if (__assertTrue__(exp, #exp)) return; } while (0)

inline bool
__assertTrue__(int exp, char *_exp)
{
    if (!exp) {
        fail("%s is true, {%s: %i}", _exp, _exp, exp);
        return true;
    }
    return false;
}


#define assertFalse(exp) do { if (__assertFalse__(exp, #exp)) return; } while (0)

inline bool
__assertFalse__(int exp, char *_exp)
{
    if (exp) {
        fail("%s is false, {%s: %i}", _exp, _exp, exp);
        return true;
    }
    return false;
}


#define assertError(exp, err) do { if (__assertError__(exp, err, #exp, #err)) return; } while (0)

#define assertOK(exp) assertError(exp, OK)

inline bool
__assertError__(tError exp, tError err, char *_exp, char *_err)
{
    if (exp != err) {
        fail("%s == %s, {%s: %i, %s: %i}", _exp, _err, _exp, exp, _err, err);
        return true;
    }
    return false;
}


#endif /* MICROTEST_H_ */
