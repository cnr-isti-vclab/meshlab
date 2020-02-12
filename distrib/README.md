# distrib folder

The distrib folder contains a set of pre-built libraries, plugins, shaders and textures that MeshLab needs to properly run. 

Running `qmake && make` from the [src](https://github.com/cnr-isti-vclab/meshlab/tree/master/src) folder of the repo, MeshLab binaries, plugins and libraries will be placed automatically in this folder.
If instead a shadow build is set, the `distrib` folder will be automatically copied inside the selected build directory, and it will contain also binaries, plugins and libraries.

After building MeshLab, the `distrib` directory will **not** contain a portable version of MeshLab, since it won't contain all the Qt's libraries needed by MeshLab. In order to deploy MeshLab, see the [install](https://github.com/cnr-isti-vclab/meshlab/tree/master/install) directory.