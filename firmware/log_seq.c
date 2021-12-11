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

#include <stdbool.h>
#include <stdint.h>

#include "log_seq.h"


struct LogSeqGroup log_seq_group = {
    .log_seq = {
        {
            .adc = &adc_group.adc[0]
        },
        {
            .adc = &adc_group.adc[1]
        }
    }
};


void
ADC0SS1IntHandler(void)
{
    ADCIntClear(ADC0_BASE, LOG_SEQ_SEQUENCE_NUM);
    LogSeqIntHandler(&log_seq_group.log_seq[0]);
}


void
ADC1SS1IntHandler(void)
{
    ADCIntClear(ADC1_BASE, LOG_SEQ_SEQUENCE_NUM);
    LogSeqIntHandler(&log_seq_group.log_seq[1]);
}

