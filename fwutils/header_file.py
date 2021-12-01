from pathlib import Path
import re


class HeaderFile:

    define_pattern = re.compile(r"^#define ([A-Z_]+) (.+)$", re.MULTILINE)
    enum_pattern = re.compile(
        r"^enum ([a-zA-Z0-9_]+) ENUM_TYPE {(.*?)};$", re.MULTILINE | re.DOTALL
    )

    def __init__(self, h_file_path: Path):
        self.h_file_path = h_file_path.resolve()
        assert self.h_file_path.exists()

        with self.h_file_path.open("r") as file:
            self.content = file.read()

        self.define = dict(self.parse_defines())
        self.enum = dict(self.parse_enums())
        self.elements = self.define | self.enum

    def parse_defines(self):
        for match in self.define_pattern.finditer(self.content):
            try:
                yield match.group(1), eval(match.group(2))
            except SyntaxError:
                pass

    def parse_enums(self):
        for match in self.enum_pattern.finditer(self.content):
            yield match.group(1), {
                element.strip(): i
                for i, element in enumerate(match.group(2).split(","))
            }

    def __getitem__(self, item):
        return self.elements.__getitem__(item)
