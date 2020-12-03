/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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

#ifndef SSI_H_
#define SSI_H_


#define SSI_BUFFER_LENGTH 1000


typedef struct SSI {
    uint16_t buffer[SSI_BUFFER_LENGTH];
    uint32_t length;
} tSSI;


extern tSSI ssi;


uint32_t SSIGetLength(tSSI *self);

void SSISetLength(tSSI *self, uint32_t length);

uint16_t *SSIGetBuffer(tSSI *self);

void SSISetDivider(tSSI *self, uint8_t predivider, uint8_t divider);

void SSIStart(tSSI *self);

void SSIStop(tSSI *self);

void SSIInit(tSSI *self);


#endif /* SSI_H_ */
