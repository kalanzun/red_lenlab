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

command packets, up to 64 bytes, from host to device
reply packets, up to 64 bytes, from device to host
data packets, 1024 bytes, from device to host only

packet header, 4 bytes
- one byte packet id (command name or reply name)
- one byte argument type (this is a helper for checking of programming errors during encoding and decoding)
- two bytes for custom use, int alignment
 */

#define LENLAB_PACKET_HEAD_LENGTH 4
#define LENLAB_PACKET_BODY_LENGTH 28 // command and reply packet

// in case of changes, please also update lenlab/protocol/message.cpp

enum Command {
    noCommand,
    init,
    getName,
    getVersion,
    setSignalSine,
    stopSignal,
    startOscilloscope,
    startTrigger,
    startLogger,
    stopLogger,
    startOscilloscopeLinearTestData,
    startTriggerLinearTestData,
    NUM_COMMANDS
};

enum Reply {
    noReply,
    Init,
    Name,
    Version,
    Signal,
    OscilloscopeData,
    Oscilloscope,
    LoggerData,
    Logger,
    Error,
    NUM_REPLIES
};

enum Type {
    noType,
    String,
    ByteArray,
    ShortArray,
    IntArray,
    NUM_TYPES
};

#endif // LENLAB_PROTOCOL_H
