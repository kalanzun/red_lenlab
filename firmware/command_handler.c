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
#include "reply_handler.h"


static uint8_t commands[8][64] __attribute__ ((aligned(4)));

struct Ring command_queue = NEW_RING(commands);


static void
start_init(struct Message *command)
{
    struct Message *reply = RingAcquire(&reply_queue);

    reply->reply = Init;
    reply->type = nullType;
    reply->argument = 0;

    RingWrite(&reply_queue);
}


static void
get_echo(struct Message *command)
{
    uint32_t i;
    struct Message *reply = RingAcquire(&reply_queue);

    for (i = 0; i < reply_queue.element_size / 4; ++i) {
        ((uint32_t *) reply)[i] = ((uint32_t *) command)[i];
    }

    RingWrite(&reply_queue);
}


static void
get_pages(struct Message *command)
{
    struct Message *page;

    ASSERT(!page_queue.has_content);
    while (page_queue.has_space) {
        page = RingAcquire(&page_queue);
        page->reply = Page;
        page->type = nullType;
        page->argument = 0;
        RingWrite(&page_queue);
    }
}


void
CommandHandlerMain(void)
{
    struct Message *command;

    if (command_queue.has_content && reply_queue.has_space) {
        command = RingRead(&command_queue);

        switch (command->command) {
        case startInit:
            start_init(command);
            break;

        case getEcho:
            get_echo(command);
            break;

        case getPages:
            get_pages(command);
            break;

        default:
            ASSERT(false);
            break;
        }

        RingRelease(&command_queue);
    }
}
