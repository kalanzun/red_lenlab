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


static uint32_t commands[8][64 / 4];

struct Ring command_queue = RING_NEW(commands);


void
CommandHandlerMain(void)
{
    uint32_t i;
    uint8_t *command;
    uint8_t *reply;

    if (!command_queue.empty && !reply_queue.full) {
        command = RingRead(&command_queue);

        reply = RingAcquire(&reply_queue);
        for (i = 0; i < command_queue.element_size; ++i) {
            reply[i] = command[i];
        }
        RingWrite(&reply_queue);

        RingRelease(&command_queue);
    }
}
