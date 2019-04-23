
from pathlib import Path

from array import array
import usb.core
import usb.backend.libusb1

import numpy as np
import matplotlib.pyplot as plt

backend = usb.backend.libusb1.get_backend(find_library=lambda x: str(Path("../libusb/MS64/dll/libusb-1.0.dll").resolve()))

assert backend

dev = usb.core.find(idVendor=0x1cbe, idProduct=0x0270, backend=backend)

assert dev

dev.set_configuration()


def startTriggerLinearTestData():
    cmd = array("B")
    cmd.append(11)
    cmd.append(0)
    cmd.append(0)
    cmd.append(0)
    return cmd


dev.write(0x1, startTriggerLinearTestData())
reply = dev.read(0x81, 100 * 1024)
data = np.ndarray((18, 1024), np.int8, reply) # checks len

# 4 byte head and 2 byte empty
# even bytes are channel 1, odd bytes are channel 2
data0 = np.cumsum(data[:, 6:1024:2]).flat
data1 = np.cumsum(data[:, 7:1024:2]).flat
plt.plot(data0)
plt.plot(data1)
# plt.plot(data[9:, 2:512].flat)
plt.grid()
plt.show()
