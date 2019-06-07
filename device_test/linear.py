#  Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
#  Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
#  
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
