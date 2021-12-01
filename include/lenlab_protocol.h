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

#ifndef LENLAB_PROTOCOL_H
#define LENLAB_PROTOCOL_H


#define LENLAB_VID 0x1CBE
#define LENLAB_PID 0x0270


/*
 * Lenlab has command, reply, and data messages.
 * A command message travels from the host to the device.
 * Reply and data messages travel from the device to the host.
 *
 * Command and reply messages are max 60 bytes.
 * On this Launchpad, the usblib bulk device driver uses 64 bytes USB packets.
 * The messages are shorter, so that the bulk driver recognizes
 * the end of a transfer and signals it (especially on the host side).
 *
 * Data messages are 1024 bytes.
 * On this microcontroller, uDMA for USB works up to 1024 bytes at once.
 * uDMA messages cannot be smaller. The bulk driver would not
 * realize the end of a transfer and would wait.
 *
 * A message has a 4 bytes head and a 56 or 1020 bytes body.
 * The head consists of
 *   - one byte message code (command code or reply code)
 *   - one byte message type (this is a helper to check for programming errors
 *     during encoding and decoding)
 *   - two bytes reference (the firmware replies with the corresponding command
 *     reference value)
 * A command or reply uses 64 bytes memory and stores the actual
 * size in the last 4 bytes.
 *
 */


#if defined(__cplusplus)
#include <stdint.h>
#define ENUM_TYPE : uint8_t
#define _Static_assert(x, y) static_assert(x, y)
#else
#define ENUM_TYPE
#endif


enum Command ENUM_TYPE {
    nullCommand,
    setUp,
    getEcho,
    getPages,
    getTicks,
    NUM_COMMANDS
};


enum Reply ENUM_TYPE {
    nullReply,
    Setup,
    Echo,
    Page,
    Tick,
    NUM_REPLIES
};


enum Type ENUM_TYPE {
    nullType,
    String,
    ByteArray,
    ShortArray,
    IntArray,
    NUM_TYPES
};


struct Message {
    union {
        enum Command command;
        enum Reply reply;
    };
    enum Type type;

    uint16_t reference;

    uint8_t body[56];

    uint32_t size;
};


_Static_assert(sizeof(struct Message) == 64, "struct Message is 64 bytes");


#endif // LENLAB_PROTOCOL_H
