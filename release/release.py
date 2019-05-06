#
# Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
# Copyright (C) 2017 Christoph Simon and the Lenlab developer team
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

"""Generate a Lenlab Release
"""

import platform

from distutils.version import LooseVersion
from os import chdir, environ, listdir, mkdir, system, walk, access, R_OK, W_OK, X_OK, remove
from os.path import abspath, dirname, join, splitext
from pathlib import Path
from re import compile
from shutil import rmtree, copy, copytree
from subprocess import call, check_output
from time import sleep
from zipfile import ZipFile


def single(iterator, message):
    result = list(iterator)
    assert len(result) == 1, message
    return result[0]


class Pattern:

    def __init__(self, pattern):
        self.pattern = compile(pattern)

    def __call__(self, data_iterator):
        for data in data_iterator:
            res = self.pattern.match(data)
            if res:
                yield res


#class Version:

    #def __init__(self, res):
        #self.res = res
        #self.version = [int(x) for x in res.groups()]

    #def __str__(self):
        #return ".".join(str(x) for x in self.version)


class Version:

    version_h = Path("..", "include", "lenlab_version.h")
    #lenlab_config_h = Path("..", "lenlab", "config.h")
    #firmware_config_h = Path("..", "firmware", "config.h")

    major_pattern = Pattern("#define MAJOR (\d+)$")
    minor_pattern = Pattern("#define MINOR (\d+)$")
    revision_pattern = Pattern("#define REVISION (\d+)$")

    def __init__(self):
        with open(self.version_h) as file:
            data = file.readlines()

        self.major = int(single(self.major_pattern(data), "No major version number found").group(1))
        self.minor = int(single(self.minor_pattern(data), "No minor version number found").group(1))

        # with open(self.lenlab_config_h) as file:
        #     self.lenlab_revision = int(single(self.revision_pattern(file), "No lenlab revision number found").group(1))
        #
        # with open(self.firmware_config_h) as file:
        #     self.firmware_revision = int(single(self.revision_pattern(file), "No lenlab revision number found").group(1))

        #self.revision = max(self.lenlab_revision, self.firmware_revision)

        self.sys = platform.system()

        if self.sys == "Windows":
            self.release_name = "lenlab_{}-{}_win".format(self.major, self.minor)#, self.revision)
        elif self.sys == "Linux":
            self.release_name = "lenlab_{}-{}_linux".format(self.major, self.minor)#, self.revision)


class QtWindows:

    base_path = Path("C:", "/Qt")
    arch = "mingw73_32"

    version_pattern = Pattern("(\d+\.\d+\.\d+)$")
    qtenv2bat_pattern = Pattern("set PATH=(.*?);(.*?);%PATH%")

    def __init__(self):
        assert access(self.base_path, R_OK), "Qt base path '{}' not found".format(self.base_path)

        self.versions = [LooseVersion(res.group(1)) for res in self.version_pattern(listdir(self.base_path))]
        assert len(self.versions) > 0, "No Qt versions found"

        self.versions.sort()
        self.version = self.versions[-1]
        self.path = self.base_path / str(self.version) / self.arch / "bin"

        with open(self.path / "qtenv2.bat") as file:
            env_path = single(list(self.qtenv2bat_pattern(file)), "Qt tools path information from qtenv2.bat not found")
        self.tools_path = env_path.group(2)


class QwtWindows:

    base_path = Path("C:", "/")

    version_pattern = Pattern("Qwt-(\d+\.\d+\.\d+)")

    def __init__(self):
        self.versions = [LooseVersion(res.group(1)) for res in self.version_pattern(listdir(self.base_path))]
        assert len(self.versions) > 0, "No Qwt found"
        self.versions.sort()
        self.version = self.versions[-1]

        self.path = self.base_path / "Qwt-{}".format(self.version)

        self.dll = self.path / "lib" / "qwt.dll"
        assert access(self.dll, R_OK), "No qwt.dll found"


class LibusbWindows:

    base_path = join("..", "libusb")
    arch = "MinGW32"

    def __init__(self):
        self.path = self.base_path
        assert access(self.path, R_OK), "No libusb found"
        self.dll = join(self.base_path, self.arch, "dll", "libusb-1.0.dll")
        assert access(self.dll, R_OK), "No libusb-1.0.dll found"


class Firmware:

    path = Path("..", "bin")

    def __init__(self, version):
        self.firmware = "lenlab_firmware_{}-{}.out".format(version.major, version.minor)#, version.firmware_revision)
        assert access(self.path / self.firmware, R_OK), "No firmware found"


class Lenlab:

    path = Path("..", "..", "build-red_lenlab-Desktop-Release", "lenlab", "app")

    def __init__(self, version):
        self.lenlab = self.path / "lenlab"
        assert access(self.lenlab, R_OK), "No lenlab found"


class LenlabWindows:

    base_path = Path("..", "..")
    arch = "MinGW_32_bit"

    def __init__(self, version, qt):
        self.path = self.base_path / "build-red_lenlab-Desktop_Qt_{}_{}_{}_{}-Release".format(
            qt.version.version[0], qt.version.version[1], qt.version.version[2], self.arch) / "lenlab" / "app" / "release"
        self.lenlab = self.path / "lenlab.exe"
        assert access(self.lenlab, R_OK), "No lenlab found"


class Doc:

    path = Path("..", "doc", "_build", "html")

    def __init__(self):
        assert access(self.path, R_OK), "No documentation found"



def build():
    version = Version()

    if version.sys == "Windows":
        qt = QtWindows()
        qwt = QwtWindows()
        libusb = LibusbWindows()
        lenlab = LenlabWindows(version, qt)
    else:
        lenlab = Lenlab(version)

    firmware = Firmware(version)
    doc = Doc()

    if not "build" in listdir():
        mkdir("build")

    build = Path("build", version.release_name)

    if version.release_name in listdir("build"):
        rmtree(build)
        sleep(0.1) # windows is not that quick

    mkdir(build)

    # Lenlab
    mkdir(build / "lenlab")

    if version.sys == "Windows":
        copy(lenlab.lenlab, build / "lenlab" / "lenlab.exe")
        copy(libusb.dll,    build / "lenlab" / "libusb-1.0.dll")
        copy(qwt.dll,       build / "lenlab" / "qwt.dll")

        # qt
        # windeployqt does not work, when called directly. It does work when called through cmd
        cmd = ["C:\Windows\System32\cmd.exe", "/C", "windeployqt",
               "-opengl", "-printsupport",
               str(build / "lenlab" / "lenlab.exe")]
        env = dict(environ)
        env["PATH"] = "{};{};{}".format(qt.path, qt.tools_path, env["PATH"])
        call(cmd, env=env)

    elif version.sys == "Linux":
        copy(lenlab.lenlab, Path("build", version.release_name, "lenlab", "lenlab"))

        ldd_parser = compile("\t([^ ]*) => ([^ ]*) \(0x[0-9a-f]*\)$")
        library_selectors = (
            compile("libqwt\.so\.6$"),
            compile("libQt5.*$"),
            compile("libpng\d+\.so\.\d+$"),
            compile("libicui18n\.so\.\d+$"),
            compile("libpcre\d+\.so\.\d+$"),
            compile("libicuuc\.so\.\d+$"),
            compile("libicudata\.so\.\d+$")
        )
        libraries = check_output(["ldd", join("build", version.release_name, "lenlab", "lenlab")], universal_newlines=True)
        for line in libraries.split("\n"):
            data = ldd_parser.match(line)
            if data:
                lib, libpath = data.groups()
                for selector in library_selectors:
                    if selector.match(lib):
                        copy(libpath, join("build", version.release_name, "lenlab", lib))
                        break
        mkdir(join("build", version.release_name, "lenlab", "platforms"))
        copy(join("/usr/lib/qt/plugins/platforms/libqxcb.so"), join("build", version.release_name, "lenlab", "platforms", "libqxcb.so"))
        for lib in ["libQt5DBus.so.5", "libQt5XcbQpa.so.5", "libstdc++.so.6", "libxcb-xinerama.so.0"]:
            copy(join("/usr/lib", lib), join("build", version.release_name, "lenlab", lib))

    else:
        raise Exception("Unknown system.")

    # Firmware
    mkdir(build / "firmware")
    copy(firmware.path / firmware.firmware, build / "firmware" / firmware.firmware)

    # Documentation
    copytree(doc.path, build / "doc")
    rmtree(build / "doc" / ".doctrees", ignore_errors=True)
    rmtree(build / "doc" / "breathe", ignore_errors=True)
    remove(build / "doc" / ".buildinfo")

    # Readme and License
    copy(Path("..", "README.md"), build / "README.md")
    copy(Path("..", "README.pdf"), build / "README.pdf")
    copy(Path("..", "LICENSE.md"), build / "LICENSE.md")

    # uniflash_windows_64
    if version.sys == "Windows":
        copytree(Path("..", "uniflash_windows_64"), build / "uniflash_windows_64")
        mkdir(build / "uniflash_windows_64" / "user_files" / "images")
        copy(firmware.path / firmware.firmware, build / "uniflash_windows_64" / "user_files" / "images" / "red_firmware.out")

    # Linux
    if version.sys == "Linux":
        mkdir(build / "linux")
        copy(Path("..", "linux", "20-lenlab.rules"), build / "linux" / "20-lenlab.rules")

    # Package
    with ZipFile(Path("build", "{}.zip".format(version.release_name)), "w") as package:
        for root, dirs, files in walk(build):
            for name in files:
                package.write(Path(root, name), Path(root, name).relative_to("build"))


def main():
    chdir(dirname(abspath(__file__)))

    build()

    #try:
        #build()
    #except Exception as detail:
        #print(detail)


if __name__ == "__main__":
    main()

