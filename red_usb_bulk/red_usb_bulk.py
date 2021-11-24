from pathlib import Path

from time import time

import usb.core
import usb.backend.libusb1

backend = usb.backend.libusb1.get_backend(find_library=lambda x: str(Path("../libusb/MinGW64/dll/libusb-1.0.dll").resolve()))
assert backend

dev = usb.core.find(idVendor=0x1cbe, idProduct=0x0270, backend=backend)
assert dev

dev.set_configuration()

size = 1_024_000

dev.write(0x1, bytes(64))
start = time()
reply = dev.read(0x81, size)
duration = time() - start

rate = size / duration
print(f"{rate / 1000} kB/s")
