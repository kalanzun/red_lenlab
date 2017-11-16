/*
 * debug.h
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "utils/uartstdio.h"


//*****************************************************************************
//
// Debug-related definitions and declarations.
//
// Debug output is available via UART0 if DEBUG is defined during build.
//
//*****************************************************************************
#ifdef DEBUG
// Map all debug print calls to UARTprintf in debug builds.
#define DEBUG_PRINT UARTprintf

#else

// Compile out all debug print calls in release builds.
#define DEBUG_PRINT while(0) ((int (*)(char *, ...))0)
#endif


#endif /* DEBUG_H_ */
