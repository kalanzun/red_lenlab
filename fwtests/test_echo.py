from red_board import RedBoard
from lenlab_protocol import protocol


def test_echo():
    board = RedBoard()

    command = bytearray(64)
    command[0] = protocol.enum["Command"]["getEcho"]
    command[1] = protocol.enum["Type"]["nullType"]
    command[2] = 0
    command[3] = 0
    for i in range(4, 64):
        command[i] = i

    board.write(command)
    reply = board.read(64)

    for i in range(64):
        assert reply[i] == command[i]
