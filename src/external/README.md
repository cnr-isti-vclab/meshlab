# External libraries for MeshLab

This folder contains a set of `cmake` configuration files to download and build external libraries required by MeshLab and its plugins.

Most of the times, `cmake` will first try to find the external libraries installed in the system, and only if they are not found, it will download them.

## Required libraries

The libraries that are required to build MeshLab are:

- Eigen
- GLEW
- easyexif

Since these libraries are required, they are bundled into the `external` directory (with the exception of eigen, which is bundled in the VCGLib subrepo).
Without these two libraries (or their relative system provided libraries), MeshLab cannot be built.

All the other libraries are optional, and they will be downloaded automatically by cmake during configuration in the `external/downloads`.

## Optional libraries

All the other libraries are optional. Cmake adopts the following strategy:

- Try to find the library in the system (if a package of the library exists). If it is found, nothing else need to be done;
- If the library is not found, try to download the source code of the library and place it into the `downloads` folder. If the download succeeds, the library is built along with MeshLab;
- If the download does not succeed, all the plugins that require that library are ignored and won't be built. 
