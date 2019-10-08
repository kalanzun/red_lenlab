Release
======================

This file briefly describes the steps to create a new release.

## Version

Update the version first, then start developing on the `develop` branch.

The version is hard coded in:

- `include/lenlab_version.h` for lenlab and the firmware
- `doc/conf.py` for the documentation
- `README.md` and `README.pdf` (only the major version 7)

## Copyright

See `release/copyright.py`

* Update the copyright statement in `copyright.py`
* The range of years should be extended to the year of the release.
* Run `copyright.py`
* Update some copyright statements manually. It says which ones in `copyright.py`

## Readme and License

Currently, the build process does not produce the pdf files for README and LICENSE. We generate them by hand and commit them.

## Firmware

The build servers cannot build the firmware, because the Code Composer Studio is too large to install and TI does not provide the compiler alone.

So for this project, we commit the firmware binaries to `bin`. The release builds pick it up from there and package it.

## Test

Please test your release first. You may create -alpha and -beta releases on any branch. Just tag the commit.

Build process
-------------

The release builds are done by build servers, appveyor.com for Windows and travis-ci.org for Linux and Mac. A detailed description of the build environments is in `build.md`.

A tag on a git commit triggers a build. The tag label defines the version. The format is `$MAJOR.$MINOR-$RELEASE`. For example `7.4-alpha3`. The build process uses the version number `7.4` to find the firmware binary. The `-alpha3` part is there, because git cannot have two tags with the same label. The last "official" release may be without a `-$RELEASE` component.

You may create the tag on the github releases page with "Draft a new release".

Note on pre-release: Appveyor will set pre-release to false, when finished. But you can set it again.

ReadTheDocs builds on every master commit. The windows build will download the documentation
from readthedocs, which should be finished by the time the tag is created and the compiler is done.

## Release

* Merge to master
* readthedocs will update
* "Draft a new release" on github
* The build servers will work and finally upload the release package to the release page on github.

