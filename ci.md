Continuous Integration
======================

- Windows: https://ci.appveyor.com/project/ChristophSimon/red-lenlab
- Linux, Mac: https://travis-ci.org/kalanzun/red_lenlab
- Documentation: https://red-lenlab.readthedocs.io

Build process
-------------

The build processes for the program trigger on a commit with a tag. The tag defines the version.
You may create the tag on the github releases page with "Draft a new release".

Note on pre-release: Appveyor will set pre-release to false, when finished.

ReadTheDocs builds on every master commit. The windows build will download the documentation
from readthedocs, which should be finished by the time the tag is created and the compiler is done.

Windows
-------

The versions of the dependencies, (Qt, qwt, libusb, ...) are hardcoded in build.py.
It downloads a specific version for each one. To update, edit build.py. 

Linux
-----

Dependencies: It uses the current packages from apt, depending on the version of the distribution. See .travis.yml.

Mac
---

Dependencies: It uses the newest version from homebrew. See .travis.yml.

