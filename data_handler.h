/*
 * data_handler.h
 *
 *  Created on: 27.10.2017
 *      Author: Christoph
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
