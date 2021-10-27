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

import os
import re
import shutil
from subprocess import call as run
import sys


def build_osx(env):
    env["PATH"] = env["HOME"] + r"/Qt/6.1/macos/bin:" + env["PATH"]

    run(["qmake", "red_lenlab.pro"], env=env)
    run(["make"], env=env)

    tag = env["APPVEYOR_REPO_TAG_NAME"]

    os.mkdir("build")
    run(["cp", "-r", "lenlab/app/lenlab.app", "build/"])
    run(["macdeployqt", "build/lenlab.app"], env=env)

    # Readme and License
    shutil.copy("README.md", "build/README.md")
    shutil.copy("README.pdf", "build/README.pdf")
    shutil.copy("LICENSE.md", "build/LICENSE.md")
    shutil.copy("LICENSE.pdf", "build/LICENSE.pdf")

    run(
        [
            "hdiutil",
            "create",
            "-volname",
            "Lenlab-" + tag,
            "-srcFolder",
            "build",
            "-ov",
            "-format",
            "UDZO",
            "Lenlab-" + tag + "-mac.dmg",
        ]
    )


def build_linux(env):
    env["PATH"] = env["HOME"] + r"/Qt/5.15/gcc_64/bin:" + env["PATH"]
    # there is a Qt6, but it did not work because glibc is too old

    run(["sudo", "apt-get", "update"])
    run(["sudo", "apt-get", "install", "-y", "libusb-1.0-0-dev"])

    run(["qmake", "red_lenlab.pro"], env=env)
    run(["make"])

    tag = env["APPVEYOR_REPO_TAG_NAME"]

    run(
        [
            "wget",
            "-c",
            "-nv",
            "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage",
        ]
    )
    run(["chmod", "a+x", "linuxdeployqt-continuous-x86_64.AppImage"])

    os.makedirs("build/usr/share/applications")
    run(["cp", "linux/lenlab.desktop", "build/usr/share/applications/"])

    os.makedirs("build/usr/share/icons/hicolor/scaleable/")
    run(["cp", "linux/lenlab.svg", "build/usr/share/icons/hicolor/scaleable/"])

    os.makedirs("build/usr/bin")
    run(["cp", "lenlab/app/lenlab", "build/usr/bin/"])

    # linuxdeployqt uses VERSION environment variable for the filename
    env["VERSION"] = tag + "-linux"
    run(
        [
            "./linuxdeployqt-continuous-x86_64.AppImage",
            "build/usr/share/applications/lenlab.desktop",
            "-appimage",
        ],
        env=env,
    )


def build_windows(env):
    env["PATH"] = r"C:\Qt\6.2\mingw81_64\bin;C:\Qt\Tools\mingw810_64\bin;" + env["PATH"]

    run(
        [
            "appveyor",
            "DownloadFile",
            "https://github.com/libusb/libusb/releases/download/v1.0.24/libusb-1.0.24.7z",
        ]
    )
    os.mkdir("libusb")
    run(["7z", "x", r"..\\libusb-1.0.24.7z"], cwd="libusb")

    run(["qmake", "red_lenlab.pro"], env=env, shell=True)
    run(["mingw32-make"], env=env, shell=True)

    tag = env["APPVEYOR_REPO_TAG_NAME"]
    result = re.compile(r"(\d)\.(\d)").match(tag)
    major = int(result.group(1))
    minor = int(result.group(2))

    release_dir_name = "Lenlab-" + tag + "-win64"

    os.makedirs(release_dir_name + "/lenlab")

    shutil.copy(
        "lenlab/app/release/lenlab.exe", release_dir_name + "/lenlab/lenlab.exe"
    )
    shutil.copy(
        "libusb/MinGW64/dll/libusb-1.0.dll", release_dir_name + "/lenlab/libusb-1.0.dll"
    )

    run(
        ["windeployqt", "-opengl", "-printsupport", "lenlab.exe"],
        cwd=release_dir_name + "/lenlab",
        env=env,
        shell=True,
    )

    # Readme and License
    shutil.copy("README.md", release_dir_name + "/README.md")
    shutil.copy("README.pdf", release_dir_name + "/README.pdf")
    shutil.copy("LICENSE.md", release_dir_name + "/LICENSE.md")
    shutil.copy("LICENSE.pdf", release_dir_name + "/LICENSE.pdf")

    # Documentation
    # Note: Do not collide with the repository directory 'red-lenlab' or with 'doc'
    run(
        [
            "appveyor",
            "DownloadFile",
            "-FileName", "html.zip",
            "https://readthedocs.org/projects/red-lenlab/downloads/htmlzip/latest/",
        ]
    )
    run(["7z", "x", "html.zip", "-ohtml"])
    shutil.move("html/red-lenlab-latest", release_dir_name + "/doc")

    # Firmware
    firmware_name = "lenlab_firmware_%s-%s.out" % (major, minor)
    os.makedirs(release_dir_name + "/firmware")
    shutil.copy("bin/" + firmware_name, release_dir_name + "/firmware/" + firmware_name)

    # uniflash_windows_64
    shutil.copytree("uniflash_windows_64", release_dir_name + "/uniflash_windows_64")
    path = os.path.join(release_dir_name, "uniflash_windows_64", "user_files", "images")
    os.mkdir(path)
    shutil.copy("bin/" + firmware_name, os.path.join(path, "red_firmware.out"))

    run(["7z", "a", release_dir_name + ".zip", release_dir_name])


def main():
    env = dict(os.environ)

    if sys.platform.startswith("linux"):
        build_linux(env)
    elif sys.platform.startswith("darwin"):
        build_osx(env)
    elif sys.platform.startswith("win32"):
        build_windows(env)
    else:
        raise ValueError("Unknown operating system")


if __name__ == "__main__":
    main()
