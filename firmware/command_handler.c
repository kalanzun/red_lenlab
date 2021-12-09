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

#include "logger.h"
#include "message.h"
#include "page_handler.h"
#include "reply_handler.h"
#include "tick.h"


static uint8_t commands[4][64] __attribute__ ((aligned(4)));

struct Ring command_queue = NEW_RING(commands);


static bool
set_up(struct Message *command)
{
    struct Message *reply = (struct Message *) RingAcquire(&reply_queue);

    setReply(reply, Setup, nullType, command->head.reference);

    RingWrite(&reply_queue);

    return true;
}


static bool
get_echo(struct Message *command)
{
    uint32_t i;

    struct Message *reply = (struct Message *) RingAcquire(&reply_queue);

    reply->size = 60;

    for (i = 0; i < 15; ++i) {
        ((uint32_t *) reply)[i] = ((uint32_t *) command)[i];
    }

    RingWrite(&reply_queue);

    return true;
}


static bool
get_pages(struct Message *command)
{
    struct Message *page;
    struct Message *reply;
    int i;

    if (!page_queue[0].empty || !page_queue[1].empty) return false;

    for (i = 0; i < 2; ++i) {
        while (page_queue[i].has_space) {
            page = (struct Message *) RingAcquire(&page_queue[i]);
            setReply(page, Page, nullType, command->head.reference);
            RingWrite(&page_queue[i]);
        }

        reply = (struct Message *) RingAcquire(&reply_queue);
        setPageQueue(reply, &page_queue[i]);
        RingWrite(&reply_queue);
    }

    return true;
}


static bool
get_ticks(struct Message *command)
{
    uint32_t interval = getInt(command, 0);
    uint32_t count = getInt(command, 1);

    // TODO locking of modules, red fw state machine
    TickStart(interval, count, command->head.reference);

    return true;
}


static bool
start_logger(struct Message *command)
{
    uint32_t interval = getInt(command, 0);

    LoggerStart(interval);

    return true;
}


static bool
stop_logger(struct Message *command)
{
    LoggerStop();

    return true;
}


void
CommandHandlerMain(void)
{
    bool success;

    struct Message *command;

    if (command_queue.has_content && reply_queue.has_space) {
        command = (struct Message *) RingPeak(&command_queue);

        switch (command->head.command) {
        case setUp:
            success = set_up(command);
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

        case startLogger:
            success = start_logger(command);
            break;

        case stopLogger:
            success = stop_logger(command);
            break;

        default:
            ASSERT(false);
            break;
        }

        if (success) {
            RingRead(&command_queue);
            RingRelease(&command_queue);
        }
    }
}
