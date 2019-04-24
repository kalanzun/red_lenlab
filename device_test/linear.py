
from pathlib import Path

from array import array
import usb.core
import usb.backend.libusb1

import numpy as np
import matplotlib.pyplot as plt

backend = usb.backend.libusb1.get_backend(find_library=lambda x: str(Path("../libusb/MS64/dll/libusb-1.0.dll").resolve()))
assert(backend)
dev = usb.core.find(idVendor=0x1cbe, idProduct=0x0270, backend=backend)

assert(dev)

dev.set_configuration()

def startOscilloscope():
    cmd = array("B")
    cmd.append(6)
    cmd.append(4)
    cmd.append(0)
    cmd.append(0)
    cmd.append(1)
    cmd.append(0)
    cmd.append(0)
    cmd.append(0)
    return cmd

def startOscilloscopeLinearTestData():
    cmd = array("B")
    cmd.append(10)
    cmd.append(0)
    cmd.append(0)
    cmd.append(0)
    return cmd


dev.write(0x1, startOscilloscopeLinearTestData())
reply = dev.read(0x81, 100 * 1024)
data = np.ndarray((20, 512), np.uint16, reply)

plt.plot(data[:10, 8:].flat)
plt.plot(data[10:, 8:].flat)
plt.grid()
plt.show()
