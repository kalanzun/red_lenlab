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


inline void
setReply(struct Message *message, enum Reply reply, enum Type type, uint16_t reference)
{
    message->reply = reply;
    message->type = type;
    message->reference = reference;
}


inline uint32_t
getInt(struct Message *message, uint32_t index)
{
    ASSERT(message->type == IntArray);
    return ((uint32_t *) &message->body)[index];
}


inline void
setInt(struct Message *message, uint32_t index, uint32_t value)
{
    ASSERT(message->type == IntArray);
    ((uint32_t *) &message->body)[index] = value;
}


#endif /* MESSAGE_H_ */
