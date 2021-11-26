import usb.core

from backend import backend
from lenlab_protocol import protocol
from lenlab_version import version


class RedBoard:
    def __init__(self):
        self.dev = usb.core.find(
            idVendor=protocol.VID, idProduct=protocol.PID, backend=backend
        )
        assert self.dev

        assert self.dev.manufacturer == "Karlsruhe Institute of Technology"
        assert self.dev.product == "Lenlab Red Board"
        assert self.dev.serial_number == version.version

        self.dev.set_configuration()

    def read(self, data):
        return self.dev.read(0x81, data)

    def write(self, data):
        return self.dev.write(0x1, data)
