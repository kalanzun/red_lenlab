/*
 * command_handler.h
 *
 */

#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

// receives USB packets and parses the commands


#include "event_queue.h"


typedef struct CommandHandler {
    tQueue command_queue;
} tCommandHandler;


extern tCommandHandler command_handler;


void CommandHandlerInit(tCommandHandler *self);

void CommandHandlerMain(tCommandHandler *self);


#endif /* COMMAND_HANDLER_H_ */
