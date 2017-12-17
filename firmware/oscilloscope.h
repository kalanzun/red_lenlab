/*
 * oscilloscope.h
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

#ifndef OSCILLOSCOPE_H_
#define OSCILLOSCOPE_H_


#define OSCILLOSCOPE_HEADER_LENGTH 8
#define OSCILLOSCOPE_PACKET_LENGTH 1024
#define OSCILLOSCOPE_QUEUE_LENGTH 16


typedef struct Oscilloscope {
    bool active;
    bool send;
    uint8_t queue[OSCILLOSCOPE_QUEUE_LENGTH][OSCILLOSCOPE_PACKET_LENGTH];
    uint32_t read;
    uint32_t write;
} tOscilloscope;


extern tOscilloscope oscilloscope;


void OscilloscopeStart(tOscilloscope *self);

void OscilloscopeMain(tOscilloscope *self);

void OscilloscopeInit(tOscilloscope *self);


#endif /* OSCILLOSCOPE_H_ */
