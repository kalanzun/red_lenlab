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

#ifndef SSI_H_
#define SSI_H_


#define SSI_BUFFER_LENGTH 1000


struct SSI {
    uint16_t buffer[SSI_BUFFER_LENGTH];
};


extern struct SSI ssi;


void SSISetDivider(uint8_t predivider, uint8_t divider);

void SSIStart(void);

void SSIStop(void);

void SSIInit(void);


#endif /* SSI_H_ */
