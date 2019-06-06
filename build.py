import os
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

    run(
        [
            "./linuxdeployqt-continuous-x86_64.AppImage",
            "build/usr/share/applications/lenlab.desktop",
            "-appimage",
        ],
    )


def main():
    os_name = os.environ.get("TRAVIS_OS_NAME", "")
    if os_name == "linux":
        build_linux()
    elif os_name == "osx":
        build_osx()
    else:
        raise ValueError("Unknown TRAVIS_OS_NAME")


if __name__ == "__main__":
    main()
