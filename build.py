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


def build_osx(env):
    # detect qwt version
    contents = os.listdir("/usr/local/Cellar/qwt")
    assert len(contents) == 1
    qwt_version = contents[0]

    # qwt config uses this (false) include path
    run(
        [
            "ln",
            "-s",
            "/usr/local/lib/qwt.framework/Headers",
            "/usr/local/Cellar/qwt/" + qwt_version + "/include",
        ]
    )

    env["PATH"] = "/usr/local/opt/qt5/bin:" + env["PATH"]

    run(["qmake", "-set", "QMAKEFEATURES", "/usr/local/opt/qwt/features"], env=env)

    run(["qmake", "red_lenlab.pro"], env=env)
    run(["make"], env=env)

    tag = env["TRAVIS_TAG"]

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
    run(["qmake", "red_lenlab.pro"])
    run(["make"])

    tag = env["TRAVIS_TAG"]
    arch = env["TRAVIS_CPU_ARCH"]

    if arch == "amd64":
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

    if arch == "arm64":
        release_name = "Lenlab-" + tag + "-linux-arm64"
        shutil.copy("lenlab/app/lenlab", release_name)


def build_windows(env):
    run(
        [
            "appveyor",
            "DownloadFile",
            "https://github.com/libusb/libusb/releases/download/v1.0.24/libusb-1.0.24.7z",
        ]
    )
    os.mkdir("libusb")
    run(["7z", "x", r"..\libusb-1.0.24.7z"], cwd="libusb")

    if not os.path.exists(r"C:\Qwt-6.1.5\features"):
        run(
            [
                "appveyor",
                "DownloadFile",
                "https://sourceforge.net/projects/qwt/files/qwt/6.1.6/qwt-6.1.6.tar.bz2",
            ]
        )
        run(["7z", "x", "qwt-6.1.6.tar.bz2"])
        run(["7z", "x", "qwt-6.1.6.tar"])
        run(["qmake", "qwt.pro"], cwd="qwt-6.1.6")
        run(["mingw32-make"], cwd="qwt-6.1.6")
        run(["mingw32-make", "install"], cwd="qwt-6.1.6")

    run(["qmake", "-set", "QMAKEFEATURES", r"C:\Qwt-6.1.6\features"])
    # Note: This path is also in .appveyor.yml, in an environment variable QWTDIR

    run(["qmake", "red_lenlab.pro"])
    run(["mingw32-make"])

    tag = env["APPVEYOR_REPO_TAG_NAME"]
    result = re.compile(r"(\d)\.(\d)").match(tag)
    major = int(result.group(1))
    minor = int(result.group(2))

    release_dir_name = "Lenlab-" + tag + "-win32"

    os.makedirs(release_dir_name + "/lenlab")

    shutil.copy(
        "lenlab/app/release/lenlab.exe", release_dir_name + "/lenlab/lenlab.exe"
    )
    shutil.copy(
        "libusb/MinGW32/dll/libusb-1.0.dll", release_dir_name + "/lenlab/libusb-1.0.dll"
    )
    shutil.copy(
        os.environ["QWTDIR"] + "/lib/qwt.dll", release_dir_name + "/lenlab/qwt.dll"
    )

    run(
        ["windeployqt", "-opengl", "-printsupport", "lenlab.exe"],
        cwd=release_dir_name + "/lenlab",
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

    if "TRAVIS_OS_NAME" in env:
        os_name = os.environ["TRAVIS_OS_NAME"]
    elif "APPVEYOR" in env:
        os_name = "windows"
    else:
        raise ValueError("Unknown CI service")

    if os_name == "linux":
        build_linux(env)
    elif os_name == "osx":
        build_osx(env)
    elif os_name == "windows":
        build_windows(env)
    else:
        raise ValueError("Unknown operating system")


if __name__ == "__main__":
    main()
