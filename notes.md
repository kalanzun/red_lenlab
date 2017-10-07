# Red Firmware

## Project Configuration

### Variables

New path variable TIVAWARE_INSTALL
New build variable TIVAWARE_INSTALL

or

vars.ini in workspace

TIVAWARE_INSTALL = C:\ti\TivaWare_C_Series-2.1.4.178

and in code composer studio File/Import/Code Composer Studio/Build Variables

Scope: Workspace

### Including TivaWare

Code composer studio project link to driverlib.lib relative to path variable TIVAWARE_INSTALL

(right click on project, Add Files, choose file and in the next dialog, there's an option to link the file into the project)

Project Properties/Build/ARM Compiler/Include Options:
${TIVAWARE_INSTALL}

## Notes

Do include stdint.h and stdbool.h before the tivaware driverlib library, else it produces errors.

```c
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/gpio.h"
```

