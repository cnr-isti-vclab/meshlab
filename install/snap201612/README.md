Snap is a cross-distribution packaging format for Linux. See
https://snapcraft.io for more information.

How to build
------------
Just type
```
snapcraft
```
from the directory containing this README file. This snap has been successfully
built in Ubuntu 17.04; Ubuntu 16.10 will not work because MeshLab needs Qt 5.7,
which is not available there. It should be possible to build MeshLab as a snap
in older Ubuntu distributions by using the [qt57
part](https://wiki.ubuntu.com/snapcraft/parts), but no efforts have been taken
in this direction since anyway the generated snap package will work on older
distributions too.

How to test the snap
--------------------
The generated snap can be installed by typing
```
snap install --dangerous ./meshlab*.snap
```
The `--dangerous` flag is needed because the snap has not been verified by the
store.

How to upload the snap to the store
-----------------------------------
A thorough guide can be found at the [snapcraft.io
site](https://snapcraft.io/docs/build-snaps/publish).

