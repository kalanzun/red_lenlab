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
import distutils.version
import os
import re
import shutil
import subprocess
import time
import zipfile


def single(iterator, message):
    result = list(iterator)
    assert len(result) == 1, message
    return result[0]


class Pattern:

    def __init__(self, pattern):
        self.pattern = re.compile(pattern)

    def __call__(self, data_iterator):
        for data in data_iterator:
            res = self.pattern.match(data)
            if res:
                yield res


class Version:

    version_h = os.path.join("..", "include", "lenlab_version.h")

    major_pattern = Pattern(r"#define MAJOR (\d+)$")
    minor_pattern = Pattern(r"#define MINOR (\d+)$")

    sys_pattern = {
        "Windows": "win",
        "Linux": "linux",
        "Darwin": "mac",
    }

    def __init__(self):
        with open(self.version_h) as file:
            data = file.readlines()

        self.major = int(single(self.major_pattern(data), "No major version number found").group(1))
        self.minor = int(single(self.minor_pattern(data), "No minor version number found").group(1))

        self.sys = platform.system()
        self.sys_name = self.sys_pattern[self.sys]

        self.release_name = "lenlab_{}-{}_{}".format(self.major, self.minor, self.sys_name)


class QtWindows:

    base_path = os.path.join("C:", "/Qt")
    arch = "mingw73_32"

    version_pattern = Pattern(r"(\d+\.\d+\.\d+)$")
    qtenv2bat_pattern = Pattern(r"set PATH=(.*?);(.*?);%PATH%")

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


class QtDarwin:

    base_path = os.path.join(os.environ["HOME"], "Qt")
    arch = "clang_64"

    version_pattern = Pattern(r"(\d+\.\d+\.\d+)$")

    def __init__(self):
        assert os.access(self.base_path, os.R_OK), "Qt base path '%s' not found" % self.base_path

        self.versions = [
            distutils.version.LooseVersion(res.group(1))
            for res in self.version_pattern(os.listdir(self.base_path))]
        assert len(self.versions) > 0, "No Qt versions found"

        self.versions.sort()
        self.version = self.versions[-1]
        self.path = os.path.join(self.base_path, str(self.version), self.arch, "bin")


class QwtWindows:

    base_path = os.path.join("C:", "/")

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

    base_path = os.path.join("..", "libusb")
    arch = "MinGW32"

    def __init__(self):
        self.path = self.base_path
        assert access(self.path, R_OK), "No libusb found"
        self.dll = join(self.base_path, self.arch, "dll", "libusb-1.0.dll")
        assert access(self.dll, R_OK), "No libusb-1.0.dll found"


class Firmware:

    path = os.path.join("..", "bin")

    def __init__(self, version):
        self.firmware = "lenlab_firmware_{}-{}.out".format(version.major, version.minor)#, version.firmware_revision)
        assert os.access(os.path.join(self.path, self.firmware), os.R_OK), "No firmware found"


class Lenlab:

    path = os.path.join("..", "..", "build-red_lenlab-Desktop-Release", "lenlab", "app")

    def __init__(self, version):
        self.lenlab = os.path.join(self.path, "lenlab")
        assert os.access(self.lenlab, os.R_OK), "No lenlab found"


class LenlabWindows:

    base_path = os.path.join("..", "..")
    arch = "MinGW_32_bit"

    def __init__(self, version, qt):
        self.path = self.base_path / "build-red_lenlab-Desktop_Qt_{}_{}_{}_{}-Release".format(
            qt.version.version[0], qt.version.version[1], qt.version.version[2], self.arch) / "lenlab" / "app" / "release"
        self.lenlab = self.path / "lenlab.exe"
        assert access(self.lenlab, R_OK), "No lenlab found"


class LenlabDarwin:

    base_path = os.path.join("..", "..")
    arch = "clang_64bit"

    def __init__(self, version, qt):
        self.path = os.path.join(
            self.base_path, "build-red_lenlab-Desktop_Qt_%s_%s_%s_%s-Release" % (
                qt.version.version[0], qt.version.version[1], qt.version.version[2], self.arch),
            "lenlab", "app")
        self.lenlab = os.path.join(self.path, "lenlab.app")
        assert os.access(self.lenlab, os.R_OK), "No lenlab found"


class Doc:

    path = os.path.join("..", "doc", "_build", "html")

    def __init__(self):
        assert os.access(self.path, os.R_OK), "No documentation found"


def build():
    version = Version()

    if version.sys == "Windows":
        qt = QtWindows()
        qwt = QwtWindows()
        libusb = LibusbWindows()
        lenlab = LenlabWindows(version, qt)
    elif version.sys == "Darwin":
        qt = QtDarwin()
        lenlab = LenlabDarwin(version, qt)
    else:
        lenlab = Lenlab(version)

    firmware = Firmware(version)
    doc = Doc()

    if "build" not in os.listdir():
        os.mkdir("build")

    build = os.path.join("build", version.release_name)

    if version.release_name in os.listdir("build"):
        shutil.rmtree(build)
        time.sleep(0.1)  # windows is not that quick

    os.mkdir(build)

    # Lenlab
    if version.sys == "Windows":
        os.mkdir(os.path.join(build, "lenlab"))

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
        mkdir(build / "lenlab" / "usr" / "bin", parents=True)
        copy(lenlab.lenlab, Path("build", version.release_name, "lenlab", "usr", "bin", "lenlab"))

        mkdir(build / "lenlab" / "usr" / "share" / "applications", parents=True)
        copy(Path("..", "linux", "lenlab.desktop"), build / "lenlab" / "usr" / "share" / "applications" / "lenlab.desktop")
        
        mkdir(build / "lenlab" / "usr" / "share" / "icons" / "hicolor" / "scaleable" / "apps", parents=True)
        copy(Path("..", "linux", "lenlab.svg"), build / "lenlab" / "usr" / "share" / "icons" / "hicolor" / "scaleable" / "apps" / "lenlab.svg")
        
        cmd = ["../../linuxdeployqt-6-x86_64.AppImage", "lenlab/usr/share/applications/lenlab.desktop", "-qmake=/usr/lib/x86_64-linux-gnu/qt5/bin/qmake", "-appimage"]
        env = dict(environ)
        env["VERSION"] = "{}.{}".format(version.major, version.minor)
        call(cmd, env=env, cwd=str(build))
        rmtree(build / "lenlab")

    elif version.sys == "Darwin":
        shutil.copytree(lenlab.lenlab, os.path.join(build, "lenlab.app"))

        env = dict(os.environ)
        env["PATH"] = "%s:%s" % (qt.path, env["PATH"])

        cmd = [
            "install_name_tool", "-change", "qwt.framework/Versions/6/qwt",
            "/usr/local/qwt-6.1.5-svn/lib/qwt.framework/Versions/6/qwt",
            "lenlab.app/Contents/MacOS/lenlab"]
        subprocess.call(cmd, env=env, cwd=build)

        cmd = ["macdeployqt", "lenlab.app"]
        subprocess.call(cmd, env=env, cwd=build)

    else:
        raise Exception("Unknown system.")

    # Firmware
    os.mkdir(build, "firmware")
    shutil.copy(os.path.join(firmware.path, firmware.firmware), os.path.join(build, "firmware", firmware.firmware))

    # Documentation
    if 0:
        shutil.copytree(doc.path, os.path.join(build, "doc"))
        shutil.rmtree(os.path.join(build, "doc", ".doctrees"), ignore_errors=True)
        shutil.rmtree(os.path.join(build, "doc", "breathe"), ignore_errors=True)
        shutil.remove(os.path.join(build, "doc", ".buildinfo"))

    # Readme and License
    shutil.copy(os.path.join("..", "README.md"), os.path.join(build, "README.md"))
    shutil.copy(os.path.join("..", "README.pdf"), os.path.join(build, "README.pdf"))
    shutil.copy(os.path.join("..", "LICENSE.md"), os.path.join(build, "LICENSE.md"))

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
    with zipfile.ZipFile(os.path.join("build", "%s.zip" % version.release_name), "w") as package:
        for root, dirs, files in os.walk(build):
            for name in files:
                assert root.startswith(build)
                package.write(os.path.join(root, name), os.path.join(root[len(build):], name))


def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    build()

    #try:
        #build()
    #except Exception as detail:
        #print(detail)


if __name__ == "__main__":
    main()

