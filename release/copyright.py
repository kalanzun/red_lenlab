#  Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
#  Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

"""Update project files with the copyright statement of this file.

Please update doc/conf.py, the /*.md files, and the about dialog by hand.

"""

import os
from pathlib import Path


class Parser:

    def __init__(self):
        self.head = list()
        self.body = list()

    def write(self, file):
        file.writelines(self.head)
        file.write("\n")
        file.writelines(self.body)


class CParser(Parser):

    @classmethod
    def parse(cls, file):
        lines = iter(file)
        line = next(lines)
        while line == "\n":
            line = next(lines)
        if line.startswith("//**"):  # do not update files with TI license
            return
        self = cls()
        if line.strip().startswith("/*"):
            while not line.strip().startswith("*/"):
                self.head.append(line)
                line = next(lines)
            self.head.append(line)
            line = next(lines)
        while line == "\n":
            line = next(lines)
        self.body.append(line)
        self.body.extend(lines)
        return self

    def set_copyright(self, copyright):
        self.head = [" * {}".format(line) for line in copyright]
        self.head.insert(0, "/*\n")
        self.head.append(" */\n")


class PyParser(Parser):

    @classmethod
    def parse(cls, file):
        self = cls()
        lines = iter(file)
        line = next(lines)
        while line == "\n":
            line = next(lines)
        while line.startswith("#"):
            self.head.append(line)
            line = next(lines)
        while line == "\n":
            line = next(lines)
        self.body.append(line)
        self.body.extend(lines)

        return self

    def set_copyright(self, copyright):
        self.head = ["#  {}".format(line) for line in copyright]

    def write(self, file):
        file.writelines(self.head)
        file.write("\n")
        file.writelines(self.body)


class RstParser(Parser):

    @classmethod
    def parse(cls, file):
        lines = iter(file)
        line = next(lines)
        while line == "\n":
            line = next(lines)
        if not line.startswith(".."):
            return
        self = cls()
        self.head.append(line)
        line = next(lines)
        while line.startswith("    ") or line == "\n":
            self.head.append(line)
            line = next(lines)
        self.body.append(line)
        self.body.extend(lines)
        return self

    def set_copyright(self, copyright):
        self.head = ["..  {}".format(copyright[0])]
        self.head.extend("    {}".format(line) for line in copyright[1:])


def parse(name):
    if name.suffix in [".py", ".pri", ".pro"]:
        with open(name) as file:
            return PyParser.parse(file)
    elif name.suffix in [".c", ".h", ".cpp"]:
        with open(name) as file:
            return CParser.parse(file)
    elif name.suffix == ".rst":
        with open(name) as file:
            return RstParser.parse(file)


class Updater:

    def __init__(self):
        with open(__file__) as file:
            parser = PyParser.parse(file)
        self.copyright = [line.lstrip("#  ") for line in parser.head]

    def update(self, name):
        source = parse(name)
        if not source:
            return
        source.set_copyright(self.copyright)
        with open(name, "w") as file:
            source.write(file)

    def update_directory(self, name, exclude=None):
        for filename in os.listdir(name):
            if exclude and filename in exclude:
                continue
            self.update(name / filename)


def main():
    updater = Updater()

    updater.update_directory(Path("../device_test"))

    updater.update_directory(Path("../firmware"))
    updater.update_directory(Path("../firmware/tests"))

    updater.update_directory(Path("../include"))

    project = Path("../lenlab")
    for name in os.listdir(project):
        path = project / name
        if path.is_dir():
            updater.update_directory(path)

    project = Path("../test")
    for name in os.listdir(project):
        path = project / name
        if path.is_dir():
            updater.update_directory(path)

    updater.update_directory(Path("../doc"), exclude=["make.py"])

    updater.update_directory(Path(".."))


if __name__ == "__main__":
    main()
