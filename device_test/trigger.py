
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


# package, 4 bytes per line
# head (reply, type, 0, last)
# preamble (state0{2}, state1{2})
# data (trigger{2}, delta0{1}, delta1{1})
# data (delta0{1}, delta1{1}, delta0{1}, delta1{1})

dev.write(0x1, startTriggerLinearTestData())
reply = dev.read(0x81, 100 * 1024)
head = np.ndarray((18, 512), np.uint16, reply)  # checks len
data = np.ndarray((18, 1024), np.int8, reply)  # checks len

# 4 byte head and 2 byte empty
# even bytes are channel 1, odd bytes are channel 2
print("trigger value", np.sum(head[:, 2]))
data0 = (head[:, 4].reshape((18, 1)) + np.cumsum(data[:, 18::2], axis=1)).flat
data1 = (head[:, 5].reshape((18, 1)) + np.cumsum(data[:, 19::2], axis=1)).flat

plt.plot(data0)
plt.plot(data1)

# plt.plot(data[:, 18::2].flat)
# plt.plot(data[:, 19::2].flat)

plt.grid()
plt.show()
