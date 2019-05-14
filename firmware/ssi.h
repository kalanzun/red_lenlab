/*
 * ssi.h
 *
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
