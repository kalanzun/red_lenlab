from pathlib import Path

import usb.core
import usb.backend.libusb1

backend = usb.backend.libusb1.get_backend(find_library=lambda x: str(Path("../libusb/MinGW64/dll/libusb-1.0.dll").resolve()))
assert backend

dev = usb.core.find(idVendor=0x1cbe, idProduct=0x0270, backend=backend)
assert dev

dev.set_configuration()


def make_command(command, type=0, argument=0):
    message = bytearray(64)
    message[0] = command
    message[1] = type
    message[2] = argument
    message[3] = argument >> 8
    return message

command = make_command(1)  # startInit
size = dev.write(0x1, command)
reply = dev.read(0x81, 64)

assert reply[0] == 1  # Init
for i in range(1, 4):
    assert reply[i] == 0


command = bytearray(64)
command[0] = 2  # getEcho
command[1] = 0  # nullType

for k in range(1000):
    for i in range(4, 64):
        command[i] = i + k % 16

    dev.write(0x1, command)
    #print(command)
    reply = dev.read(0x81, 64)
    #print(reply)

    for i in range(64):
        assert reply[i] == command[i]


command = make_command(3)  # getPages
dev.write(0x1, command)
size = 24 * 1024
while size:
    print(size)
    reply = dev.read(0x81, size)
    size -= len(reply)

