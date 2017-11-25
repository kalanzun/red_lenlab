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

from os import chdir, environ, listdir, mkdir, system, walk, access, R_OK, W_OK, X_OK, remove
from os.path import abspath, dirname, join, splitext
from pathlib import Path
from re import compile
from shutil import rmtree, copy, copytree
from subprocess import call, check_output
from time import sleep
from zipfile import ZipFile


version = (7, 0, 2)

sys = platform.system()

if sys == "Windows":
    release_name = "lenlab_{}-{}-{}_win".format(version[0], version[1], version[2])
elif sys == "Linux":
    release_name = "lenlab_{}-{}-{}_linux".format(version[0], version[1], version[2])
else:
    raise RuntimeError()


def single(iterator, message):
    result = list(iterator)
    assert len(result) == 1, message
    return result[0]


class Pattern:

    def __init__(self, pattern):
        self.pattern = compile(pattern)

    def find(self, data_iterator):
        for data in data_iterator:
            res = self.pattern.match(data)
            if res:
                yield res


class Version:

    def __init__(self, res):
        self.res = res
        self.version = [int(x) for x in res.groups()]

    def __str__(self):
        return ".".join(str(x) for x in self.version)


class Config:

    def __init__(self):
        self.exception = None

        try:
            self.config()
        except Exception as exception:
            print(exception)
            self.exception = exception


class QtWindows(Config):

    base_path = r"C:\Qt"
    arch = "mingw53_32"

    version_pattern = Pattern(r"(\d+)\.(\d+)\.(\d+)$")
    qtenv2bat_pattern = Pattern(r"set PATH=(.*?);(.*?);%PATH%")

    def config(self):
        assert access(self.base_path, R_OK), "Qt base path '{}' not found".format(self.base_path)

        self.versions = [Version(x) for x in self.version_pattern.find(listdir(self.base_path))]
        assert len(self.versions) > 0, "No Qt versions found"

        self.versions.sort()
        self.version = self.versions[-1]
        self.path = join(self.base_path, str(self.version), self.arch, "bin")

        with open(join(self.path, "qtenv2.bat")) as file:
            env_path = single(list(self.qtenv2bat_pattern.find(file)), "Qt tools path information from qtenv2.bat not found")
        self.tools_path = env_path.group(2)


class QwtWindows(Config):

    base_path = "C:\\"

    pattern = Pattern(r"Qwt-(\d+)\.(\d+)\.(\d+)")

    def config(self):
        self.versions = [Version(x) for x in self.pattern.find(listdir(self.base_path))]
        assert len(self.versions) > 0, "No Qwt found"
        self.versions.sort()
        self.version = self.versions[-1]

        self.path = join(self.base_path, self.version.res.group(0))

        self.dll = join(self.path, "lib", "qwt.dll")
        assert access(self.dll, R_OK), "No qwt.dll found"


class LibusbWindows(Config):

    base_path = join("..", "libusb")
    arch = "MinGW32"

    def config(self):
        self.path = self.base_path
        assert access(self.path, R_OK), "No libusb found"
        self.dll = join(self.base_path, self.arch, "dll", "libusb-1.0.dll")
        assert access(self.dll, R_OK), "No libusb-1.0.dll found"


class LenlabWindows(Config):

    base_path = ".."

    pattern = Pattern("build-lenlab(.*?)-Desktop(?:_Qt_(\d)_(\d)_(\d)_(.*?))?-Release$")

    def config(self):
        path = single(self.pattern.find(listdir(self.base_path)), "No lenlab found").group(0)
        self.path = join(self.base_path, path)
        self.lenlab = join(self.base_path, path, "release", "lenlab.exe")
        assert access(self.lenlab, R_OK), "No lenlab found"


class Doc(Config):

    path = Path("..", "doc", "_build", "html")

    def config(self):
        assert access(self.path, R_OK), "No documentation found"


class Firmware(Config):

    firmware = Path("..", "firmware", "Release", "red_firmware.out")

    def config(self):
        assert access(self.firmware, R_OK), "No firmware found"


def build():
    if not "build" in listdir():
        mkdir("build")

    if release_name in listdir("build"):
        rmtree(join("build", release_name))
        sleep(0.1) # windows is not that quick

    mkdir(join("build", release_name))

    qt = QtWindows()
    if qt.exception:
        return

    qwt = QwtWindows()
    if qwt.exception:
        return

    libusb = LibusbWindows()
    if libusb.exception:
        return

    lenlab = LenlabWindows()
    if lenlab.exception:
        return

    firmware = Firmware()
    if firmware.exception:
        return

    doc = Doc()
    if doc.exception:
        return

    # Lenlab
    mkdir(join("build", release_name, "lenlab"))

    if sys == "Windows":
        copy(lenlab.lenlab, join("build", release_name, "lenlab", "lenlab.exe"))
        copy(libusb.dll,    join("build", release_name, "lenlab", "libusb-1.0.dll"))
        copy(qwt.dll,       join("build", release_name, "lenlab", "qwt.dll"))

        # qt
        # windeployqt does not work, when called directly. It does work when called through cmd
        cmd = ["C:\Windows\System32\cmd.exe", "/C", "windeployqt",
               "-opengl", "-printsupport",
               join("build", release_name, "lenlab", "lenlab.exe")]
        env = dict(environ)
        env["PATH"] = "{};{};{}".format(qt.path, qt.tools_path, env["PATH"])
        call(cmd, env=env)

    elif sys == "Linux":
        pass

        #copy(join("..", lenlab_build_directory, "LenLab"), join("build", release_name, "lenlab", "LenLab"))
        #copy(join("..", "lenlab", "linux", "LenLab.sh"), join("build", release_name, "lenlab", "LenLab.sh"))
        #libraries = check_output(["ldd", join("build", release_name, "lenlab", "LenLab")], universal_newlines=True)
        #for line in libraries.split("\n"):
            #data = ldd_parser.match(line)
            #if data:
                #lib, libpath = data.groups()
                #for selector in library_selectors:
                    #if selector.match(lib):
                        #copy(libpath, join("build", release_name, "lenlab", lib))
                        #break
        #mkdir(join("build", release_name, "lenlab", "platforms"))
        #copy(join("/usr/lib/qt/plugins/platforms/libqxcb.so"), join("build", release_name, "lenlab", "platforms", "libqxcb.so"))
        #for lib in ["libQt5DBus.so.5", "libQt5XcbQpa.so.5", "libstdc++.so.6", "libxcb-xinerama.so.0"]:
            #copy(join("/usr/lib", lib), join("build", release_name, "lenlab", lib))
    else:
        raise RuntimeError()

    # Firmware
    mkdir(join("build", release_name, "firmware"))
    copy(firmware.firmware, join("build", release_name, "firmware", "lenlab_firmware_{}-{}-{}.bin".format(version[0], version[1], version[2])))

    if sys == "Windows":
        copytree(join("..", "uniflash_windows_64"), join("build", release_name, "uniflash_windows_64"))
        mkdir(join("build", release_name, "uniflash_windows_64", "user_files", "images"))
        copy(firmware.firmware, join("build", release_name, "uniflash_windows_64", "user_files", "images", "red_firmware.out"))

    # Documentation
    copytree(doc.path, join("build", release_name, "doc"))
    rmtree(join("build", release_name, "doc", ".doctrees"))
    rmtree(join("build", release_name, "doc", "breathe"))
    remove(join("build", release_name, "doc", ".buildinfo"))

    # Readme and License
    #copy(join("..", "manual", "readme.pdf"), join("build", release_name, "readme.pdf"))
    copy(join("..", "LICENSE.md"), join("build", release_name, "LICENSE.md"))

    # Package
    with ZipFile(Path("build", "{}.zip".format(release_name)), "w") as package:
        for root, dirs, files in walk(Path("build", release_name)):
            for name in files:
                package.write(Path(root, name), Path(root, name).relative_to("build"))


def main():
    chdir(dirname(abspath(__file__)))
    build()


if __name__ == "__main__":
    main()

