from pathlib import Path

from header_file import HeaderFile


class Version(HeaderFile):
    def __init__(self):
        super().__init__(Path("../include/lenlab_version.h"))

        self.MAJOR = int(self.define["LENLAB_MAJOR"])
        self.MINOR = int(self.define["LENLAB_MINOR"])

    @property
    def version(self):
        return f"{self.MAJOR}.{self.MINOR}"


version = Version()
