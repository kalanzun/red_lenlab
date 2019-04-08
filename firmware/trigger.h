/*
 * trigger.h
 *
 */

#ifndef TRIGGER_H_
#define TRIGGER_H_


#include "memory.h"
#include "osc_seq.h"


#define TRIGGER_FILTER_LENGTH 8


typedef struct Trigger {

    tMemory *memory;
    tOscSeqGroup *seq_group;

    bool lock;

    bool trigger;
    uint16_t filter[TRIGGER_FILTER_LENGTH];
    uint16_t index;
    uint16_t state;
    bool wait;
    bool active;
    bool save;
    uint8_t count;
    uint8_t post_count;

    tRing ring;

} tTrigger;


extern tTrigger trigger;


tError TriggerStart(tTrigger *self, uint32_t samplerate);

tError TriggerStop(tTrigger *self);

void TriggerMain(tTrigger *self);

void TriggerInit(tTrigger *self, tMemory *memory, tOscSeqGroup *seq_group);


#endif /* TRIGGER_H_ */
