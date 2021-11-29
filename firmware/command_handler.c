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

#include <stdbool.h>
#include <stdint.h>

#include "command_handler.h"

#include "message.h"
#include "reply_handler.h"
#include "tick.h"


static uint8_t commands[4][64] __attribute__ ((aligned(4)));

struct Ring command_queue = NEW_RING(commands);


static bool
start_init(struct Message *command)
{
    struct Message *reply = RingAcquire(&reply_queue);

    setReply(reply, Init, nullType, command->reference);

    RingWrite(&reply_queue);

    return true;
}


static bool
get_echo(struct Message *command)
{
    uint32_t i;

    struct Message *reply = RingAcquire(&reply_queue);

    for (i = 0; i < reply_queue.element_size / 4; ++i) {
        ((uint32_t *) reply)[i] = ((uint32_t *) command)[i];
    }

    RingWrite(&reply_queue);

    return true;
}


static bool
get_pages(struct Message *command)
{
    struct Message *page;

    if (page_queue.has_content) return false;

    while (page_queue.has_space) {
        page = RingAcquire(&page_queue);
        setReply(page, Page, nullType, command->reference);
        RingWrite(&page_queue);
    }

    return true;
}


static bool
get_ticks(struct Message *command)
{
    uint32_t interval = getInt(command, 0);
    uint32_t count = getInt(command, 1);

    // TODO locking of modules, red fw state machine
    TickStart(interval, count, command->reference);

    return true;
}


void
CommandHandlerMain(void)
{
    bool success;

    struct Message *command;

    if (command_queue.has_content && reply_queue.has_space) {
        command = RingRead(&command_queue);

        switch (command->command) {
        case startInit:
            success = start_init(command);
            break;

        case getEcho:
            success = get_echo(command);
            break;

        case getPages:
            success = get_pages(command);
            break;

        case getTicks:
            success = get_ticks(command);
            break;

        default:
            ASSERT(false);
            break;
        }

        if (success) RingRelease(&command_queue);
    }
}
