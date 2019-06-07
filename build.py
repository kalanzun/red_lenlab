import os
import re
import shutil
from subprocess import call as run


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

    def __init__(self, major, minor):
        self.major = major
        self.minor = minor

    def __str__(self):
        return "%i.%i" % (self.major, self.minor)


def version():
    version_h = os.path.join("include", "lenlab_version.h")

    major_pattern = Pattern(r"#define MAJOR (\d+)$")
    minor_pattern = Pattern(r"#define MINOR (\d+)$")

    with open(version_h) as file:
        data = file.readlines()

    major = int(single(major_pattern(data), "No major version number found").group(1))
    minor = int(single(minor_pattern(data), "No minor version number found").group(1))

    with open("VERSION", "w") as file:
        file.write(str(major))
        file.write(".")
        file.write(str(minor))

    return Version(major, minor)


def build_osx(version):
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

    #tag = env["TRAVIS_TAG"]
    #if not tag:
    #    return

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
            "Lenlab-" + str(version) + "-mac.dmg",
        ]
    )


def build_linux(version):
    run(["qmake", "red_lenlab.pro"])
    run(["make"])

    env = dict(os.environ)
    #tag = env["TRAVIS_TAG"]
    #if not tag:
    #    return

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
    env["VERSION"] = str(version) + "-linux"
    run(
        [
            "./linuxdeployqt-continuous-x86_64.AppImage",
            "build/usr/share/applications/lenlab.desktop",
            "-appimage",
        ],
        env=env,
    )


def build_windows(version):
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

    env = dict(os.environ)
    #tag = env.get("APPVEYOR_REPO_TAG_NAME", None)
    #if not tag:
    #    return

    release_dir_name = "Lenlab-" + str(version) + "-win32"

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
    run(["7z", "a", release_dir_name + ".zip", release_dir_name])


def main():
    env = dict(os.environ)

    if "TRAVIS_OS_NAME" in env:
        os_name = os.environ["TRAVIS_OS_NAME"]
    elif "APPVEYOR" in env:
        os_name = "windows"
    else:
        raise ValueError("Unknown CI service")

    v = version()

    if os_name == "linux":
        build_linux(v)
    elif os_name == "osx":
        build_osx(v)
    elif os_name == "windows":
        build_windows(v)
    else:
        raise ValueError("Unknown operating system")


if __name__ == "__main__":
    main()
