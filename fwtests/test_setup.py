import pytest
import usb.util

from fwutils.red_board import RedBoard
from fwutils.lenlab_protocol import lenlab_protocol

from .message import command, get_reference, set_reference


@pytest.fixture()
def board():
    board = RedBoard()
    yield board
    # explicitly free the resources, otherwise pytest would be too quick with the next test
    usb.util.dispose_resources(board.dev)


@pytest.fixture()
def setup():
    return command(lenlab_protocol["Command"]["setUp"])


def test_setup(board: RedBoard, setup: bytearray):
    set_reference(setup, 123)
    board.write(setup)
    reply = board.read(64)
    assert get_reference(reply) == 123
