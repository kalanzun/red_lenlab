/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
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

#ifndef REPLY_HANDLER_H_
#define REPLY_HANDLER_H_


#include "event_queue.h"


typedef struct ReplyHandler {
    tQueue reply_queue;
    tQueue logger_queue;
    tQueue oscilloscope_queue;
} tReplyHandler;


extern tReplyHandler reply_handler;


inline void
ReplyHandlerInit(tReplyHandler *self, uint8_t *reply_buffer, uint8_t *shared_buffer)
{
    QueueInit(&self->reply_queue, reply_buffer);
    QueueInit(&self->logger_queue, shared_buffer);
    QueueInit(&self->oscilloscope_queue, shared_buffer);
}


#endif /* REPLY_HANDLER_H_ */
