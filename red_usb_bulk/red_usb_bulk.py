from pathlib import Path

from time import time

import usb.core
import usb.backend.libusb1

backend = usb.backend.libusb1.get_backend(find_library=lambda x: str(Path("../libusb/MinGW64/dll/libusb-1.0.dll").resolve()))
assert backend

dev = usb.core.find(idVendor=0x1cbe, idProduct=0x0270, backend=backend)
assert dev

dev.set_configuration()

size = 1_024 * 2000
command = bytearray(64)
command[0] = 1  # send 2000 blocks à 1024 bytes
dev.write(0x1, command)
received = 0
start = time()
while size:
    reply = dev.read(0x81, size)
    size -= len(reply)
    received += len(reply)
duration = time() - start

rate = received / duration
print(f"{rate / 1000} kB/s")
