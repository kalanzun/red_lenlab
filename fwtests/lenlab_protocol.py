from pathlib import Path

from header_file import HeaderFile


class Protocol(HeaderFile):
    def __init__(self):
        super().__init__(Path("../include/lenlab_protocol.h"))

        self.VID = int(self.define["LENLAB_VID"], 16)
        self.PID = int(self.define["LENLAB_PID"], 16)


protocol = Protocol()
