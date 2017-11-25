/*
 * data_handler.h
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

#ifndef DATA_HANDLER_H_
#define DATA_HANDLER_H_


#include "data_queue.h"


typedef struct DataHandler {
    tDataQueue data_queue;
} tDataHandler;


extern tDataHandler data_handler;


void DataHandlerMain(void);

void DataHandlerInit(void);

#endif /* DATA_HANDLER_H_ */
