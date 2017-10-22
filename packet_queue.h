/*
 * packet_queue.h
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#ifndef PACKET_QUEUE_H_
#define PACKET_QUEUE_H_


#define PACKET_QUEUE_SIZE 4
#define PACKET_QUEUE_PAYLOAD_SIZE 64


typedef struct {
    uint32_t size;
    uint8_t payload[PACKET_QUEUE_PAYLOAD_SIZE];
} tPacket;


typedef struct {
    tPacket packets[PACKET_QUEUE_SIZE];
    uint32_t read;
    uint32_t write;
} tPacketQueue;


inline bool
PacketQueueEmpty(tPacketQueue *self)
{
    return self->read == self->write;
}


inline bool
PacketQueueFull(tPacketQueue *self)
{
    return (self->write+1) % PACKET_QUEUE_SIZE == self->write;
}


inline tPacket*
PacketQueueRead(tPacketQueue *self)
{
    return self->packets + self->read;
}


inline void
PacketQueueReadDone(tPacketQueue *self)
{
    self->read = (self->read + 1) % PACKET_QUEUE_SIZE;
}


inline tPacket*
PacketQueueWrite(tPacketQueue *self)
{
    return self->packets + self->write;
}

inline void
PacketQueueWriteDone(tPacketQueue *self)
{
    self->write = (self->write + 1) % PACKET_QUEUE_SIZE;
}


inline void
PacketQueueInit(tPacketQueue *self)
{
    self->read = 0;
    self->write = 0;
}


#endif /* PACKET_QUEUE_H_ */
