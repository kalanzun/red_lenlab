/*
 * reply_handler.h
 *

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef REPLY_HANDLER_H_
#define REPLY_HANDLER_H_

// sends single USB packets


#include "event_queue.h"


#define REPLY_TYPE_BOOL 1
#define REPLY_TYPE_UINT8 2
#define REPLY_TYPE_UINT16 3
#define REPLY_TYPE_UINT32 4
#define REPLY_TYPE_STRING 5


typedef struct ReplyHandler {
    tQueue reply_queue;
} tReplyHandler;


// Singleton
extern tReplyHandler reply_handler;


void ReplyHandlerInit(void);

void ReplyHandlerMain(void);


#endif /* REPLY_HANDLER_H_ */
