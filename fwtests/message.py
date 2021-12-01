from array import array


def command(code, type=0, size=0):
    # body size
    message = array("B", [code, type, 0, 0])
    message.extend(range(size))
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
