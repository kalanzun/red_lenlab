from time import time

import pytest
import usb.util
from usb.core import USBTimeoutError

from red_board import RedBoard
from lenlab_protocol import protocol
from lenlab_version import version


@pytest.fixture()
def board():
    board = RedBoard()
    yield board
    # explicitly free the resources, otherwise pytest would be too quick with the next test
    usb.util.dispose_resources(board.dev)


def command(code):
    message = bytearray(64)
    for i in range(64):
        message[i] = i

    message[0] = code
    message[1] = 0  # nullType

    return message


def set_index(message, index):
    message[2] = index & 0xFF
    message[3] = (index >> 8) & 0xFF


@pytest.fixture()
def echo():
    return command(protocol["Command"]["getEcho"])


@pytest.fixture()
def pages():
    return command(protocol["Command"]["getPages"])


@pytest.fixture()
def ticks():
    return command(protocol["Command"]["getTicks"])


def test_usb_descriptor(board: RedBoard):
    assert board.dev.manufacturer == "Karlsruhe Institute of Technology"
    assert board.dev.product == "Lenlab Red Board"
    assert board.dev.serial_number == f"{version['LENLAB_MAJOR']}.{version['LENLAB_MINOR']}"


def test_echo(board: RedBoard, echo: bytearray):
    board.write(echo)
    reply = board.read(64)
    assert reply == echo


def test_echo_repeatedly(board: RedBoard, echo: bytearray):
    for i in range(2048):
        set_index(echo, i)
        board.write(echo)
        reply = board.read(64)
        assert reply == echo


def test_pages(board: RedBoard, pages: bytearray):
    board.write(pages)
    size = len(board.read(24 * 1024))
    assert size == 24 * 1024


def test_pages_repeatedly(board: RedBoard, pages: bytearray):
    for i in range(16):
        board.write(pages)
        size = len(board.read(24 * 1024))
        assert size == 24 * 1024


def test_autoset_by_dma(board: RedBoard, echo: bytearray, pages: bytearray):
    board.write(pages)  # this did set autoset
    assert len(board.read(24 * 1024)) == 24 * 1024
    for i in range(32):
        set_index(echo, i)
        board.write(echo)  # and echo failed
        reply = board.read(64)
        assert reply == echo


def test_interleaved(board: RedBoard, echo: bytearray, pages: bytearray):
    for i in range(16):
        board.write(pages)
        size = len(board.read(24 * 1024))
        assert size == 24 * 1024

        set_index(echo, i)
        board.write(echo)
        reply = board.read(64)
        assert reply == echo


def test_transfer_speed(board: RedBoard, echo: bytearray):
    start = time()
    size = 0
    for i in range(2048):
        board.write(echo)
        size += len(board.read(64))
    speed = size / (time() - start)
    # the limiting factor is the alternating send and receive of small packets
    assert speed > 200_000  # kB/s


def test_dma_transfer_speed(board: RedBoard, pages: bytearray):
    start = time()
    size = 0
    for i in range(16):
        board.write(pages)
        size += len(board.read(24 * 1024))
    speed = size / (time() - start)
    assert speed > 800_000  # kB/s


def test_usb_blocks(board: RedBoard, echo: bytearray):
    tx = 0
    try:
        while True:
            set_index(echo, tx)
            board.write(echo)
            tx += 1
    except USBTimeoutError:
        # queues full
        assert tx > 12  # command_queue and reply_queue
        for rx in range(tx):
            set_index(echo, rx)
            reply = board.read(64)
            assert reply == echo

    # firmware recovers
    for i in range(32):
        set_index(echo, i)
        board.write(echo)
        reply = board.read(64)
        assert reply == echo


def test_dma_tx_queue(board: RedBoard, pages: bytearray):
    board.write(pages)
    board.write(pages)

    size = len(board.read(24 * 1024))
    assert size == 24 * 1024

    size = len(board.read(24 * 1024))
    assert size == 24 * 1024


def test_dma_transfer_speed_queued(board: RedBoard, pages: bytearray):
    start = time()
    size = 0
    board.write(pages)
    for i in range(32):
        board.write(pages)
        size += len(board.read(24 * 1024))
    size += len(board.read(24 * 1024))
    speed = size / (time() - start)
    assert speed > 800_000  # kB/s


def test_ticks(board: RedBoard, ticks: bytearray):
    count = 512
    set_index(ticks, count)
    board.write(ticks)
    size = 64 * count
    while size:
        size -= len(board.read(size))


@pytest.mark.slow
def test_ticks_long_time(board: RedBoard, ticks: bytearray):
    # the firmware crashes if the reply_queue is too short
    # firmware version 7 did crash in this scenario
    count = 20_000
    set_index(ticks, count)
    board.write(ticks)
    size = 64 * count
    while size:
        size -= len(board.read(size))
