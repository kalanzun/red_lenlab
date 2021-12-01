from array import array

import pytest
import usb.util

from fwutils.red_board import RedBoard

from .message import command, get_reference, set_reference


@pytest.fixture()
def board():
    board = RedBoard()
    yield board
    # explicitly free the resources, otherwise pytest would be too quick with the next test
    usb.util.dispose_resources(board.dev)


@pytest.fixture()
def setup():
    return command("setUp")


def test_setup(board: RedBoard, setup: array):
    set_reference(setup, 12345)
    board.write(setup)
    reply = board.read(64)
    assert get_reference(reply) == 12345
