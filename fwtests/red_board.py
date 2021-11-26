import usb.core

from backend import backend
from lenlab_protocol import protocol


class RedBoard:
    def __init__(self):
        self.dev = usb.core.find(
            idVendor=protocol["LENLAB_VID"], idProduct=protocol["LENLAB_PID"], backend=backend
        )
        assert self.dev

        self.dev.set_configuration()

    def read(self, data):
        return self.dev.read(0x81, data)

    def write(self, data):
        return self.dev.write(0x1, data)
