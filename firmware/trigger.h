/*
 * trigger.h
 *
 */

#ifndef TRIGGER_H_
#define TRIGGER_H_


#include "osc_seq.h"


#define TRIGGER_FILTER_LENGTH 8


typedef struct Trigger {

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

tError TriggerStop(tTrigger *self); // internal and for tests

void TriggerMain(tTrigger *self, bool enable_usb);

void TriggerInit(tTrigger *self);


#endif /* TRIGGER_H_ */
