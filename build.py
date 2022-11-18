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
from pathlib import Path
from multiprocessing import cpu_count
from shutil import copy, copytree
from subprocess import run
import sys


def print_header(headline: str):
    print()
    print("-" * 80)
    print(headline)
    print("-" * 80)
    print()


def path_str(path: Path):
    if path.is_symlink():
        return f"{path.name} -> {path.readlink().name}"
    else:
        return path.name


def print_versions(name: str, prefix: Path, pattern: str = "*"):
    print_header(f"Available {name} versions")
    for node in prefix.glob(pattern):
        print(f"- {path_str(node)}")


def print_qt_version(qmake: Path):
    print_header("Qt version")
    versions = run([qmake, "--version"], capture_output=True, text=True)
    print(versions.stdout)


def build_linux(tag: str):
    print_versions("Qt", Path.home() / "Qt")
    print_versions("Python", Path.home(), "venv*")

    # there is a Qt6, but it did not work because glibc is too old
    qmake = Path.home() / "Qt/latest/gcc_64/bin/qmake"
    print_qt_version(qmake)

    print_header("install")
    run(["sudo", "apt-get", "update"])
    # run(["sudo", "apt-get", "upgrade", "-y"])  # it takes way too long
    run(["sudo", "apt-get", "install", "-y", "libgl1-mesa-dev", "libusb-1.0-0-dev"])

    print_header("build")
    run([qmake, "red_lenlab.pro"])
    run(["make", f"-j{cpu_count()}"])

    print_header("download linuxdeployqt")
    linuxdeployqt_url = (
        "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/"
        "linuxdeployqt-continuous-x86_64.AppImage"
    )
    run(
        [
            "wget",
            "-c",
            "-nv",
            linuxdeployqt_url,
        ]
    )
    linuxdeployqt = Path("linuxdeployqt-continuous-x86_64.AppImage").absolute()
    linuxdeployqt.chmod(0o700)

    print_header("deploy")
    dest = Path("build/usr/share/applications")
    dest.mkdir(parents=True)
    copy("linux/lenlab.desktop", dest)

    dest = Path("build/usr/share/icons/hicolor/scaleable/")
    dest.mkdir(parents=True)
    copy("linux/lenlab.svg", dest)

    dest = Path("build/usr/bin")
    dest.mkdir(parents=True)
    copy("lenlab/app/lenlab", dest)

    # linuxdeployqt uses VERSION environment variable for the filename
    env = dict(os.environ)
    env["VERSION"] = f"{tag}-linux"
    env["PATH"] = f"{qmake.parent}:{env['PATH']}"
    run(
        [
            linuxdeployqt,
            "build/usr/share/applications/lenlab.desktop",
            "-appimage",
        ],
        env=env,
    )


def build_osx(tag: str):
    print_versions("Qt", Path.home() / "Qt")
    print_versions("Python", Path.home(), "venv*")

    qmake = Path.home() / "Qt/6.2/macos/bin/qmake"
    macdeployqt = Path.home() / "Qt/6.2/macos/bin/macdeployqt"
    print_qt_version(qmake)

    print_header("build")
    run([qmake, "red_lenlab.pro"])
    run(["make", f"-j{cpu_count()}"])

    print_header("deploy")
    release_dir = Path("build")
    copytree("lenlab/app/lenlab.app", release_dir / "lenlab.app")
    run([macdeployqt, release_dir / "lenlab.app"])

    print_header("Readme and License")
    copy("README.md", release_dir)
    copy("README.pdf", release_dir)
    copy("LICENSE.md", release_dir)
    copy("LICENSE.pdf", release_dir)

    print_header("package")
    run(
        [
            "hdiutil",
            "create",
            "-volname",
            f"Lenlab-{tag}",
            "-srcFolder",
            release_dir,
            "-ov",
            "-format",
            "UDZO",
            f"Lenlab-{tag}-mac.dmg",
        ]
    )


def build_windows(tag: str):
    print_versions("Qt", Path(r"C:\Qt"))
    print_versions("Python", Path(r"C:"), "Python*")

    env = dict(os.environ)

    qmake = Path(r"C:\Qt\6.4.0\mingw81_64\bin\qmake.exe")
    make = Path(r"C:\Qt\Tools\mingw810_64\bin\mingw32-make.exe")
    env["PATH"] = f"{qmake.parent};{make.parent};{env['PATH']}"
    windeployqt = qmake.parent / "windeployqt.exe"
    print_qt_version(qmake)

    print_header("download libusb")
    run(
        [
            "appveyor",
            "DownloadFile",
            "https://github.com/libusb/libusb/releases/download/v1.0.26/libusb-1.0.26-binaries.7z",
        ],
    )
    libusb_dir = Path("libusb")
    libusb_dir.mkdir()
    run(
        ["7z", "x", r"..\\libusb-1.0.26-binaries.7z"],
        cwd=libusb_dir,
    )

    print_header("build")
    run(
        [qmake, "red_lenlab.pro"],
        env=env,
    )
    run(
        [make, f"-j{cpu_count()}"],
        env=env,
    )

    print_header("deploy")
    release_dir = Path("build")
    release_dir.mkdir()
    print(f"{release_dir=}")

    dest = release_dir / "lenlab"
    dest.mkdir()
    copy("lenlab/app/release/lenlab.exe", dest / "lenlab.exe")
    copy("libusb/MinGW64/dll/libusb-1.0.dll", dest / "libusb-1.0.dll")
    run(
        [windeployqt, "lenlab.exe"],
        cwd=dest,
        env=env,
    )

    print_header("Readme and License")
    copy("README.md", release_dir)
    copy("README.pdf", release_dir)
    copy("LICENSE.md", release_dir)
    copy("LICENSE.pdf", release_dir)

    print_header("package")
    run(["7z", "a", Path(f"Lenlab-{tag}-win.zip").absolute(), fr"*"], cwd=release_dir)


def main():
    env = os.environ
    if env["APPVEYOR_REPO_TAG"] == "true":
        tag = env["APPVEYOR_REPO_TAG_NAME"]
    else:
        tag = "dev"

    print_header("Lenlab CI build script")
    print(f"Tag: {tag}")
    print(f"Platform: {sys.platform}")
    print(f"Python: {sys.version}")

    if sys.platform.startswith("linux"):
        build_linux(tag)
    elif sys.platform.startswith("darwin"):
        build_osx(tag)
    elif sys.platform.startswith("win32"):
        build_windows(tag)
    else:
        raise ValueError("Unknown operating system")


if __name__ == "__main__":
    main()
