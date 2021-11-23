from os import fdopen
from pathlib import Path
from tempfile import mkstemp
from time import time

import serial

import ccs


def build_and_flash():
    firmware = ccs.build(Path("..") / "red_usb_cdc")

    fd, name = mkstemp(suffix=".out")
    with fdopen(fd, "wb") as firmware_file:
        firmware_file.write(firmware)

    firmware_path = Path(name)

    ccs.flash(firmware_path)
    firmware_path.unlink()


def test_red_usb_cdc():
    #build_and_flash()
    ser = serial.Serial("COM5")
    start = time()
    for i in range(10_000):
        packet = ser.read(64)
    duration = time() - start
    print(f"{640 / duration} kB/s")
    ser.close()
