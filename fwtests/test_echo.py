from time import time

import pytest
import usb.util
from usb.core import USBTimeoutError

from fwutils.red_board import RedBoard
from fwutils.lenlab_protocol import lenlab_protocol
from fwutils.lenlab_version import lenlab_version


@pytest.fixture()
def board():
    board = RedBoard()
    yield board
    # explicitly free the resources, otherwise pytest would be too quick with the next test
    usb.util.dispose_resources(board.dev)


def command(code, type=0):
    message = bytearray(64)

    message[0] = code
    message[1] = type

    for i in range(4, 64):
        message[i] = i

    return message


def get_reference(message):
    return message[2] + (message[3] << 8)


def set_reference(message, reference):
    message[2] = reference & 0xFF
    message[3] = (reference >> 8) & 0xFF


def set_int(message, index, value):
    message[4 + 4 * index] = value & 0xFF
    message[4 + 4 * index + 1] = (value >> 8) & 0xFF
    message[4 + 4 * index + 2] = (value >> 16) & 0xFF
    message[4 + 4 * index + 3] = (value >> 24) & 0xFF


@pytest.fixture()
def echo():
    return command(lenlab_protocol["Command"]["getEcho"])


@pytest.fixture()
def pages():
    return command(lenlab_protocol["Command"]["getPages"])


@pytest.fixture()
def ticks():
    return command(lenlab_protocol["Command"]["getTicks"], lenlab_protocol["Type"]["IntArray"])


def test_usb_descriptor(board: RedBoard):
    assert board.dev.manufacturer == "Karlsruhe Institute of Technology"
    assert board.dev.product == "Lenlab Red Board"
    assert board.dev.serial_number == f"{lenlab_version['LENLAB_MAJOR']}.{lenlab_version['LENLAB_MINOR']}"


def test_echo(board: RedBoard, echo: bytearray):
    board.write(echo)
    reply = board.read(64)
    assert reply == echo


def test_echo_repeatedly(board: RedBoard, echo: bytearray):
    for i in range(2048):
        set_reference(echo, i)
        board.write(echo)
        reply = board.read(64)
        assert reply == echo


def test_pages(board: RedBoard, pages: bytearray):
    board.write(pages)
    reply = board.read(24 * 1024)
    assert get_reference(reply) == 0
    assert len(reply) == 24 * 1024


def test_pages_repeatedly(board: RedBoard, pages: bytearray):
    for i in range(16):
        set_reference(pages, i)
        board.write(pages)
        reply = board.read(24 * 1024)
        assert get_reference(reply) == i
        assert len(reply) == 24 * 1024


def test_autoset_by_dma(board: RedBoard, echo: bytearray, pages: bytearray):
    board.write(pages)  # this did set autoset
    reply = board.read(24 * 1024)
    assert get_reference(reply) == 0
    assert len(reply) == 24 * 1024
    for i in range(32):
        set_reference(echo, i)
        board.write(echo)  # and echo failed
        reply = board.read(64)
        assert reply == echo


def test_interleaved(board: RedBoard, echo: bytearray, pages: bytearray):
    for i in range(16):
        board.write(pages)
        reply = board.read(24 * 1024)
        assert get_reference(reply) == 0
        assert len(reply) == 24 * 1024

        set_reference(echo, i)
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


def test_dma_tx_queue(board: RedBoard, pages: bytearray):
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
    interval = 1  # ms
    count = 512
    set_int(ticks, 0, interval)
    set_int(ticks, 1, count)
    board.write(ticks)
    size = 64 * count
    while size:
        size -= len(board.read(size))


@pytest.mark.slow
def test_ticks_long_time(board: RedBoard, ticks: bytearray):
    # the firmware crashes if the reply_queue is too short
    # firmware version 7 did crash in this scenario
    interval = 1  # ms
    count = 20_000
    set_int(ticks, 0, interval)
    set_int(ticks, 1, count)
    board.write(ticks)
    size = 64 * count
    while size:
        size -= len(board.read(size))
