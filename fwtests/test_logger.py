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
def start_logger():
    return command("startLogger", "IntArray", body_size=4)


@pytest.fixture()
def stop_logger():
    return command("stopLogger")


def test_logger(board: RedBoard, start_logger: array, stop_logger: array):
    interval = 1  # ms
    count = 512
    set_int(start_logger, 0, interval)
    board.write(start_logger)
    size = 24 * count
    while size:
        size -= len(board.read(size))
    board.write(stop_logger)
