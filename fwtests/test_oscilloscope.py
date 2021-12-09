from array import array
from time import time

import pytest
import usb.util
from usb.core import USBTimeoutError

from fwutils.red_board import RedBoard
from fwutils.lenlab_version import lenlab_version

from .message import command, get_reference, set_reference, set_int


@pytest.fixture()
def board():
    board = RedBoard()
    yield board
    # explicitly free the resources, otherwise pytest would be too quick with the next test
    usb.util.dispose_resources(board.dev)


@pytest.fixture()
def start_oscilloscope():
    return command("startOscilloscope", "IntArray", body_size=4)


def test_oscilloscope(board: RedBoard, start_oscilloscope: array):
    oversampling = 1  # 1 MHz / 1
    set_int(start_oscilloscope, 0, oversampling)
    board.write(start_oscilloscope)
    size = 20 * 1024
    while size:
        size -= len(board.read(size))
