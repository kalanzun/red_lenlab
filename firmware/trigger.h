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

void TriggerMain(tTrigger *self, bool enable_reply);

tError TriggerLinearTestData(tTrigger *self);

void TriggerInit(tTrigger *self);


#endif /* TRIGGER_H_ */
