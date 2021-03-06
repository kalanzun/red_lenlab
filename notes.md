# Red Firmware

## Project Configuration

### Including TivaWare

Code composer studio project link to driverlib.lib relative to path variable TIVAWARE_INSTALL

(right click on project, Add Files, choose file and in the next dialog, there's an option to link the file into the project)

Project Properties/Build/ARM Compiler/Include Options:
${TIVAWARE_INSTALL}

## Notes

### Including

Do include stdint.h and stdbool.h before the tivaware driverlib library, else it produces errors.

```c
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/gpio.h"
```

### uDMA

uDMA cannot copy from const.

## Firmware structure

### Component

A component defines a struct for all the global variables, with the same name and a prefix t and defines one global instance.

```c
// data_handler.h

typedef struct DataHandler {
  
} tDataHandler;

extern tDataHandler data_handler;
```

It may define two functions for initialization and mainloop events.

```c
void DataHandlerMain(void);

void DataHandlerInit(void);
```

Init initializes the hardware of the component and the global struct.

Main is called from the main loop and checks for new events.

If a component has more than one instance, the struct is passed to the functions every time.

```c
void DataHandlerMain(tDataHandler *self);

void DataHandlerInit(tDataHandler *self);
```

### Hardware components

- Peripherals
- USB
- ADC
- uDMA (general configuration; the component dependant configuration is within the compontent module)

Hardware components handle the configuration and operation of a hardware module of the μC.

### Software components

- Command handler
- Data handler
- Reply handler
- Main

Software components implement the functions and handle data flow between hardware components.

### Queues

Components use queues to pass data to each other. The queue supports one producer and one consumer only. It cannot distinguish different event sources and returns the events strictly in order. But the produces may be running in interrupt context and the consumer in the main context or the other way round.

Queues use a fixed size ring buffer to store the events.

Events and queues are therefore component dependant types

* tADCQueue, tADCEvent
* tQueue, tEvent (event_queue.h for command hander and reply handler)
* tDataQueue, tDataEvent

### Data flow

A DMA channel reads data from the ADC and the ADC component appends it to an ADCQueue. The DataHandler reads ADCQueue in the main context and copies the data to a DataQueue. It may apply compression. USB reads the DataQueue and starts a DMA transfer for each event.

### Memory

The Hardware components own a little memory of their own for DMA.

* ADC: 2 buffers for 1000 values each, ping pong DMA
* SSI: 1 buffer for about 500 values, ping pong DMA from a single buffer
* USB: 4 packets of 64 bytes, send or receive

A memory module owns the rest of the memory and the active component may allocate it. Software components are the data logger or oscilloscope.

A component can copy data from the ADC into the memory

USB can use DMA to send the memory to the host

The memory is in packet layout for USB DMA with 1024 byte packet bursts.

### State machine

startOscilloscope: Oscilloscope component tries to acquire memory, it tries to acquire ADC

sends reply to host

ADC fills up the memory

when trigger condition hits, it fills up the buffer and releases ADC

it starts USB DMA of the memory, USB eventually releases the memory

#### Memory interface

void * MemoryAllocate

uint8 * memory #blocks 1024

struct memory provides read and write pointers for convenience

MemorySend(uint32 start, uint32 count)

A packet is a memory block of 1024 bytes. Send may start from any block and sends a continous chain of blocks

## Protocol

4 bytes header, up to 60 bytes data for commands, up to 1020 bytes data for replies.

Note: 64 bytes is a single USB packet, 1024 bytes is the maximum transfer size at once with uDMA.

### Command header

* 1 byte command code
* 1 byte command variant
* 2 bytes command specifc header information, i.e. length of command packet

### Reply header

* 1 byte reply code
* 1 byte reply variant (i.e. encoding of data)
* 2 bytes reply specific header information, i.e. length of reply packet

## Testing

A component produces know test data and fills the ADC queue with it. DataHandler processes the data. Another component reads the DataQueue with higher priority than the USBDevice. It test the data.

Test runs for a fixed time, is started by command handler, and sends back a result through reply handler.

# USB Device

New PID. The former Lenlab board had a different configuration and so computers, which saw the old device, cannot assign the driver anymore.

The old Lenlab won't find the new device, the new Lenlab won't find the old ones.

## Microsoft OS Descriptor

https://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/481401

