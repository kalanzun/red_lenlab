# Build

This file describes the build environment on the build servers for release builds and instructions for the setup of a local build environment for development. Your local environment should follow the settings of the build servers.

## Dependencies

- Qt (and MinGW on Windows)
- qwt
- libusb
- Python (for the build scripts) and Sphinx (for the documentation)
- TI Code Composer Studio
- TI TivaWare for TM4C Series

## Build servers

- Windows: https://ci.appveyor.com/project/ChristophSimon/red-lenlab
- Linux, Mac: https://travis-ci.org/kalanzun/red_lenlab
- Documentation: https://red-lenlab.readthedocs.io

### Windows

Build environment configuration in `.appveyor.yml`

Virtual Machine Image: Visual Studio 2015

Note: We use the MinGW compiler of the Qt package

Third party software download and install in `build.py`. The versions are defined there.

- Qt: Latest and MinGW 7.3.0 32 bit compiler
- qwt: Own build from source, version 6.1.4, hard coded in `build.py`
- libusb: Binary download from project home http://libusb.info (SourceForge), version 1.0.22, hard coded in `build.py`
- Python: Build environment default is version 2.7.16

### Linux

Build environment configuration in `.travis.yml`

Virtual Machine Image: Ubuntu 18.04 LTS Bionic Beaver

The build environment configuration specifies the virtual machine image. It will use the default version of each software of that distribution.

Compiler: gcc

Note: AppImage requires the oldest supported Ubuntu LTS

### Mac

Build environment configuration in `.travis.yml`: Xcode 12.5, macOS 11.3

Compiler: clang

The configuration does not specify the exact version of the dependencies. If Travis CI or Homebrew update the default version, the build will use the new version.

Currently:

- qt5: Homebrew latest version is 5.15.2
- qwt: Homebrew latest version is 6.1.6

## Local build environment

### Windows

#### Qt

Download and run windows Installer

https://qt.io/download-open-source/

Note: Open source version - you may skip account creation

Select

* Qt / MinGW 7.3.0 32 bit
* Tools / MinGW 7.3.0 32 bit

To build and run lenlab or the tests, open `red_lenlab.pro` in Qt Creator and click the play button (after installing the rest of the dependencies).

#### qwt

Download windows source files (the zip archive)

https://sourceforge.net/projects/qwt/files/qwt/

Start Menu -> Qt -> MinGW 7.3.0 32 bit Command Prompt

qwt will install to C:\Qwt-$VERSION by default

Installation instructions: http://qwt.sourceforge.net/qwtinstall.html

- unzip

- cd qwt-$VERSION

- qmake qwt.pro

- mingw32-make -j4

- mingw32-make install

**register qwt with the qt build system**

`qmake -set QMAKEFEATURES C:\Qwt-$VERSION\features`

This allows the project file to get all build settings with `CONFIG += qwt`.

For running lenlab, Qt Creator additionally needs the library paths. The Qwt path to run lenlab in Qt Creator is hard coded in `lenlab.pro`.

You may delete the source directory after installing.

#### libusb

http://libusb.info

Downloads -> Latest Windows Binaries

Extract file to the project directory (repository root), name `libusb`, remove the version number.

#### Python

Anaconda or standard python.

#### Anaconda

https://www.anaconda.com/download/

`conda install sphinx_rtd_theme`

#### Standard Python

`pip install sphinx sphinx_rtd_theme`

### Linux

Lenlab builds should work with the default development packages of qt5, qwt and libusb and default compiler of the distribution.

### Raspberry Pi 4

Ubuntu: https://cdimage.ubuntu.com/releases/18.04.5/release/

`ssh ubuntu@ubuntu`

`sudo apt update`

`sudo apt upgrade`

`sudo apt install build-essential qt5-default libqwt-qt5-dev libqt5svg5-dev libqt5opengl5-dev libusb-1.0-0-dev pkg-config`

`sudo reboot`

`git clone https://github.com/kalanzun/red_lenlab.git`

`cd red_lenlab`

`qmake red_lenlab.pro`

`make -j4`

The executable is `lenlab/app/lenlab`

### TI Code Composer Studio

It runs on Windows, Linux and Mac.

Download von www.ti.com/tool/CCSTUDIO

Virenscanner dekativeieren, dieser verhindert unter Umständen das Kopieren einiger Dateien.

Standard-Pfad `c:\ti`

TM4C12x ARM Cortex-M4F core-based MCUs

Standard-Einstellungen

TI XDS Debug Probe Support

Tiva/Stellaris ICDI Debug Probe

#### Tiva Ware

http://www.ti.com/tool/SW-TM4C

SW-TM4C: TivaWare for C Series Software (Complete)

Im nächsten Fenster: TivaWare for TM4C Series

TI Account notwendig

Installieren im Standard-Pfad

After installation, register the TivaWare library in the Code Composer Studio Workspace.

red lenlab reads the TivaWare path from a variable named TIVAWARE_INSTALL.

See "Creating Variable at Workspace Level" in [Portable Projects](http://software-dl.ti.com/ccs/esd/documents/ccs_portable-projects.html)

To create a workspace level Build or Path variable:

- Go to menu **Window → Preferences → Code Composer Studio → Build → Variables**.
- Click the **Add** button to define a new variable.
- Specify the **Variable name**: TIVAWARE_INSTALL
- **Type**: Directory
- and **Value**: `c:\ti\TivaWare...`
- Click **Apply and Close**.

#### Red lenlab firmware project import

To work on the firmware, import the firmware project into Code Composer Studio.

File / Import / Code Composer Studio / CCS Projects

Directory `red_lenlab/firmware`

Do not tick copy.

The build settings use `TIVAWARE_INSTALL` to link files and should work right away.

If it complains about target configuration not set, right click on target Configurations/Tiva TM4C1230H6PM.ccxml and select set as default target Configuration.

#### UART terminal for debug messages

View / Terminal

In the terminal window select Open Terminal, 115200 baud, no parity bit, 1 stop bit.