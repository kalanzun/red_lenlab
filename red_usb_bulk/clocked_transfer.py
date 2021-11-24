from pathlib import Path

from time import time

import usb.core
import usb.backend.libusb1

backend = usb.backend.libusb1.get_backend(find_library=lambda x: str(Path("../libusb/MinGW64/dll/libusb-1.0.dll").resolve()))
assert backend

dev = usb.core.find(idVendor=0x1cbe, idProduct=0x0270, backend=backend)
assert dev

dev.set_configuration()

command = bytearray(64)
command[0] = 2  # send 20 thousand packets, one per millisecond, 20 seconds
dev.write(0x1, command)

count = 20_000
start = time()
while count:
    # dev.read may raise a timeout exception
    reply = dev.read(0x81, 64)
    assert len(reply) == 64
    count -= 1
duration = time() - start
print(f"{duration} s")
