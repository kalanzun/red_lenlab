# Release

This file briefly describes the steps to create a new release.

## Version

Update the version first, then start developing on the `develop` branch.

The Lenlab App and the firmware compare their version (both components $MAJOR.$MINOR).

The version is hard coded in:

- `include/lenlab_version.h` for lenlab and the firmware
- `doc/conf.py` for the documentation
- `README.md` and `README.pdf` (only the major version 7)

## Dependencies

The package `flash-$VERSION-win.zip` depends on an installed version of uniflash.
Test, if it works with the current uniflash from TI.

## Copyright

See `release/copyright.py`

* Update the copyright statement in `copyright.py`
* The range of years should be extended to the year of the release.
* Run `copyright.py`
* Update some copyright statements manually. It says which ones in `copyright.py`

## Readme and License

Currently, the build process does not produce the pdf files for README and LICENSE.
We generate them by hand and commit them.

## Test

Please test your release first. You may create -alpha and -beta releases on any branch.

## Build process

The release builds are done by build servers. A detailed description of the build environments is in `build.md`.

Creating a new release on github triggers the build process. More detailed, creating a new release
creates and pushes and new tag to the repository. The new tag triggers the build servers.

Just creating a new tag triggers the build servers as well, but CircleCI fails to deploy
if the github release does not exist yet.

You may create the release on the github releases page with "Draft a new release".

Note on prerelease: Appveyor will set prerelease to true, when finished. If everything is fine,
remove the setting on github.

Additionally, the build servers run for every push to main, but do not deploy.

ReadTheDocs builds on every main commit.
