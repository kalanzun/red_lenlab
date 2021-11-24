from os import fdopen
from pathlib import Path
from tempfile import mkstemp
from time import time, sleep

import serial

import ccs


def build_and_flash():
    firmware = ccs.build(Path())

    fd, name = mkstemp(suffix=".out")
    with fdopen(fd, "wb") as firmware_file:
        firmware_file.write(firmware)

    firmware_path = Path(name)

    ccs.flash(firmware_path, Path("targetConfigs") / "Tiva TM4C123GH6PM.ccxml")
    firmware_path.unlink()

    sleep(6)  # wait for the USB device to appear


def test_red_usb_cdc():
    count = 10_000
    size = 64

    build_and_flash()

    ser = serial.Serial("COM5")
    start = time()
    for i in range(count):
        packet = ser.read(size)
    duration = time() - start
    rate = size * count / duration
    print(f"{rate / 1000} kB/s")
    ser.close()
    assert rate > 400_000
