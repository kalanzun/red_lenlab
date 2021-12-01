from array import array

from fwutils.lenlab_protocol import lenlab_protocol


def command(code, type=0, body_size=0):
    if isinstance(code, str):
        code = lenlab_protocol["Command"][code]
    if isinstance(type, str):
        type = lenlab_protocol["Type"][type]
    message = array("B", [code, type, 0, 0])
    message.extend(range(body_size))
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
