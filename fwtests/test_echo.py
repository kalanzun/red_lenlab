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
def echo():
    return command("getEcho", body_size=56)


@pytest.fixture()
def pages():
    return command("getPages")


@pytest.fixture()
def ticks():
    return command("getTicks", "IntArray", body_size=8)


def test_usb_descriptor(board: RedBoard):
    assert board.dev.manufacturer == "Karlsruhe Institute of Technology"
    assert board.dev.product == "Lenlab Red Board"
    assert (
        board.dev.serial_number
        == f"{lenlab_version['LENLAB_MAJOR']}.{lenlab_version['LENLAB_MINOR']}"
    )


def test_echo(board: RedBoard, echo: array):
    board.write(echo)
    reply = board.read(64)
    assert reply == echo


def test_echo_repeatedly(board: RedBoard, echo: array):
    for i in range(2048):
        set_reference(echo, i)
        board.write(echo)
        reply = board.read(64)
        assert reply == echo


def test_pages(board: RedBoard, pages: array):
    board.write(pages)
    reply = board.read(24 * 1024)
    assert get_reference(reply) == 0
    assert len(reply) == 24 * 1024


def test_pages_repeatedly(board: RedBoard, pages: array):
    for i in range(16):
        set_reference(pages, i)
        board.write(pages)
        reply = board.read(24 * 1024)
        assert get_reference(reply) == i
        assert len(reply) == 24 * 1024


def test_autoset_by_dma(board: RedBoard, echo: array, pages: array):
    board.write(pages)  # this did set autoset
    reply = board.read(24 * 1024)
    assert get_reference(reply) == 0
    assert len(reply) == 24 * 1024
    for i in range(32):
        set_reference(echo, i)
        board.write(echo)  # and echo failed
        reply = board.read(64)
        assert reply == echo


def test_interleaved(board: RedBoard, echo: array, pages: array):
    for i in range(16):
        board.write(pages)
        reply = board.read(24 * 1024)
        assert get_reference(reply) == 0
        assert len(reply) == 24 * 1024

        set_reference(echo, i)
        board.write(echo)
        reply = board.read(64)
        assert reply == echo


def test_transfer_speed(board: RedBoard, echo: array):
    start = time()
    size = 0
    for i in range(2048):
        board.write(echo)
        size += len(board.read(64))
    speed = size / (time() - start)
    # the limiting factor is the alternating send and receive of small packets
    assert speed > 200_000  # kB/s


def test_dma_transfer_speed(board: RedBoard, pages: array):
    start = time()
    size = 0
    for i in range(16):
        board.write(pages)
        size += len(board.read(24 * 1024))
    speed = size / (time() - start)
    assert speed > 800_000  # kB/s


def test_usb_blocks(board: RedBoard, echo: array):
    tx = 0
    try:
        while True:
            set_reference(echo, tx)
            board.write(echo)
            tx += 1
    except USBTimeoutError:
        # queues full
        assert tx > 12  # command_queue and reply_queue

    for rx in range(tx):
        set_reference(echo, rx)
        reply = board.read(64)
        assert reply == echo

    # firmware recovers
    for i in range(32):
        set_reference(echo, i)
        board.write(echo)
        reply = board.read(64)
        assert reply == echo


def test_dma_tx_queue(board: RedBoard, pages: array):
    set_reference(pages, 100)
    board.write(pages)
    set_reference(pages, 110)
    board.write(pages)

    reply = board.read(24 * 1024)
    assert get_reference(reply) == 100
    assert len(reply) == 24 * 1024

    reply = board.read(24 * 1024)
    assert get_reference(reply) == 110
    assert len(reply) == 24 * 1024


def test_dma_transfer_speed_queued(board: RedBoard, pages: array):
    start = time()
    size = 0
    board.write(pages)
    for i in range(32):
        board.write(pages)
        size += len(board.read(24 * 1024))
    size += len(board.read(24 * 1024))
    speed = size / (time() - start)
    assert speed > 800_000  # kB/s


def test_ticks(board: RedBoard, ticks: array):
    interval = 1  # ms
    count = 512
    set_int(ticks, 0, interval)
    set_int(ticks, 1, count)
    board.write(ticks)
    size = 8 * count
    while size:
        size -= len(board.read(size))


@pytest.mark.slow
def test_ticks_long_time(board: RedBoard, ticks: array):
    # the firmware crashes if the reply_queue is too short
    # firmware version 7 did crash in this scenario
    interval = 1  # ms
    count = 20_000
    set_int(ticks, 0, interval)
    set_int(ticks, 1, count)
    board.write(ticks)
    size = 8 * count
    while size:
        size -= len(board.read(size))
