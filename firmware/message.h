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

#ifndef MESSAGE_H_
#define MESSAGE_H_


#include "lenlab_protocol.h"


struct Message {
    struct Head head;
    union {
        struct {
            uint8_t body[56];
            uint32_t size;
        };
        struct Ring *page_queue;
    };
};


_Static_assert(sizeof(struct Message) == 64, "struct Message is 64 bytes");


inline void
setReply(struct Message *message, enum Reply reply, enum Type type, uint16_t reference)
{
    message->head.reply = reply;
    message->head.type = type;
    message->head.reference = reference;

    message->size = sizeof(struct Head);
}


inline struct Ring*
getPageQueue(struct Message *message)
{
    ASSERT(message->head.reply == nullReply);
    ASSERT(message->head.type == PageQueue);
    ASSERT(message->head.reference == 0);

    return message->page_queue;
}


inline void
setPageQueue(struct Message *message, struct Ring *page_queue)
{
    message->head.reply = nullReply;
    message->head.type = PageQueue;
    message->head.reference = 0;

    message->page_queue = page_queue;
}


inline uint32_t
getInt(struct Message *message, uint32_t index)
{
    ASSERT(message->head.type == IntArray);
    return ((uint32_t *) &message->body)[index];
}


inline void
setInt(struct Message *message, uint32_t index, uint32_t value)
{
    ASSERT(message->head.type == IntArray);
    ((uint32_t *) &message->body)[index] = value;
}


#endif /* MESSAGE_H_ */
