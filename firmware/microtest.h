/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
