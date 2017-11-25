# Build

## Dependencies

### Windows

#### Qt 5.9

Download and run windows Installer

https://qt.io/download-open-source/

Note: Open source version - you may skip account creation

Select

* Qt 5.9.2 / MinGW 5.3.0 32 bit
* Tools / MinGW 5.3.0

#### qwt 6.1.3

Download windows source files (the zip archive)

https://sourceforge.net/projects/qwt/files/qwt/

Qt -> Command Prompt

qwt will install to C:\Qwt-6.1.3, default settings are fine

Installation instructions http://qwt.sourceforge.net/qwtinstall.html

unzip

cd qwt-6.1.3 

qmake qwt.pro

mingw32-make -j4

mingw32-make install

**register qwt with qt build system**

qmake -set QMAKEFEATURES C:\Qwt-6.1.3\features

This allows the project file to get all build settings with CONFIG += qwt

For running, Qt Creator additionally needs the library paths

Qwt path to run lenlab in Qt Creator is hard coded in lenlab.pro

You may delete the source directory after installing.

#### libusb

http://libusb.info

Downloads -> Latest Windows Binaries

Extract file to project directory, libusb

/libusb

#### Anaconda

https://www.anaconda.com/download/

sphinx, breathe, doxygen, 

`conda install -c conda-forge breathe`

`conda install sphinx_rtd_theme`

`pip install sphinxcontrib-mermaid`

doxygen.org, download and install

#### TI Code Composer Studio

### Linux

#### Qt 5.9

System installation on

Arch Linux, 

#### qwt

System installation on

Arch Linux,

## Documentation

* Sphinx http://www.sphinx-doc.org
* Breathe https://github.com/michaeljones/breathe
* Doxygen http://doxygen.org
* sphinxcontrib-mermaid https://github.com/mgaitan/sphinxcontrib-mermaid

## Build

Open lenlab.pro in Qt Creator

Click build or run

## Deploy

Build packages for distribution

deploy script

