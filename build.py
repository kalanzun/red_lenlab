import os
import shutil
from subprocess import call as run


def build_osx():
    # qwt config uses this (false) include path
    run(
        [
            "ln",
            "-s",
            "/usr/local/lib/qwt.framework/Headers",
            "/usr/local/Cellar/qwt/6.1.3_4/include",
        ]
    )

    env = dict(os.environ)
    env["PATH"] = "/usr/local/opt/qt/bin:" + env["PATH"]

    run(["qmake", "-set", "QMAKEFEATURES", "/usr/local/opt/qwt/features"], env=env)

    run(["qmake", "red_lenlab.pro"], env=env)
    run(["make"], env=env)

    os.mkdir("build")
    run(["cp", "-r", "lenlab/app/lenlab.app", "build/"])
    run(["macdeployqt", "build/lenlab.app"], env=env)

    run(
        [
            "hdiutil",
            "create",
            "-volname",
            "Lenlab-7.4",
            "-srcFolder",
            "build",
            "-ov",
            "-format",
            "UDZO",
            env["RELEASE_FILE_NAME"],
        ]
    )


def build_linux():
    run(["qmake", "red_lenlab.pro"])
    run(["make"])

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
    run(
        [
            "./linuxdeployqt-continuous-x86_64.AppImage",
            "build/usr/share/applications/lenlab.desktop",
            "-appimage",
        ]
    )


def build_windows():
    run(
        [
            "appveyor",
            "DownloadFile",
            "https://github.com/libusb/libusb/releases/download/v1.0.22/libusb-1.0.22.7z",
        ]
    )
    os.mkdir("libusb")
    run(["7z", "x", r"..\libusb-1.0.22.7z"], cwd="libusb")

    if not os.path.exists(r"C:\Qwt-6.1.4\features"):
        run(
            [
                "appveyor",
                "DownloadFile",
                "https://sourceforge.net/projects/qwt/files/qwt/6.1.4/qwt-6.1.4.tar.bz2",
            ]
        )
        run(["7z", "x", "qwt-6.1.4.tar.bz2"])
        run(["7z", "x", "qwt-6.1.4.tar"])
        run(["qmake", "qwt.pro"], cwd="qwt-6.1.4")
        run(["mingw32-make"], cwd="qwt-6.1.4")
        run(["mingw32-make", "install"], cwd="qwt-6.1.4")

    run(["qmake", "-set", "QMAKEFEATURES", r"C:\Qwt-6.1.4\features"])

    run(["qmake", "red_lenlab.pro"])
    run(["mingw32-make"])

    release_dir_name = "Lenlab-" + os.environ["VERSION"] + "-win32"

    os.makedirs(release_dir_name + "/lenlab")
    shutil.copy("lenlab/app/release/lenlab.exe", release_dir_name + "/lenlab/lenlab.exe")
    shutil.copy(
        "libusb/MinGW32/dll/libusb-1.0.dll", release_dir_name + "/lenlab/libusb-1.0.dll"
    )
    shutil.copy(os.environ["QWTDIR"] + "/lib/qwt.dll", release_dir_name + "/lenlab/qwt.dll")

    run(
        ["windeployqt", "-opengl", "-printsupport", "lenlab.exe"],
        cwd=release_dir_name+"/lenlab",
    )
    run(["7z", "a", os.environ["RELEASE_FILE_NAME"], release_dir_name])


def main():
    if "TRAVIS_OS_NAME" in os.environ:
        os_name = os.environ["TRAVIS_OS_NAME"]
    elif "APPVEYOR" in os.environ:
        os_name = "windows"
    else:
        raise ValueError("Unknown CI service")

    if os_name == "linux":
        build_linux()
    elif os_name == "osx":
        build_osx()
    elif os_name == "windows":
        build_windows()
    else:
        raise ValueError("Unknown operating system")


if __name__ == "__main__":
    main()
