from pathlib import Path

import usb.core
import usb.backend.libusb1

backend = usb.backend.libusb1.get_backend(find_library=lambda x: str(Path("../libusb/MinGW64/dll/libusb-1.0.dll").resolve()))
assert backend

dev = usb.core.find(idVendor=0x1cbe, idProduct=0x0270, backend=backend)
assert dev

dev.set_configuration()

command = bytearray(64)
for i in range(64):
    command[i] = 32 + i

for k in range(1000):
    dev.write(0x1, command)
    reply = dev.read(0x81, 64)

    for i in range(64):
        assert reply[i] == 32 + i
