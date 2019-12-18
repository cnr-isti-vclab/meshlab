## Compiling MeshLab

1. install Qt >= 5.9, be sure to select additional packages "script" and "xmlpatterns"
2. clone meshlab repo
3. clone vcglib repo (_devel_ branch) at the same level of meshlab 
4. compile `src/external/external.pro`, 
5. compile `src/meshlab_full.pro`

### Dependencies
You need Qt5.9 and we assume that we have qtcreator installed and you know how to build something from a `*.pro` qt file (either from qtcreator, or from the command line). As external dependencies MeshLab uses the _vcglib_ c++ library for most of the mesh processing tasks; so you need to clone it from github ( http://github.com/cnr-isti-vclab/vcglib/ ) in a folder named `vcglib` at the same level of the folder `meshlab`. To be clear your folder structure should be something quite similar to:

    yourdevelfolder/
     |
     ├──meshlab
     │   ├──docs
     │   ├──README.md
     │   ├──src
     │   ├──...
     │   └──...
     └──vcglib
         ├──apps
         ├──doc
         ├──eigenlib
         ├──...
         └──...

All the include paths inside the `.pro` assume this relative positioning of the `meshlab` and `vcglib` folders. Please note that, given the fact that the development of the vcglib and meshlab are often intermixed, if you compile the devel branch of the meshlab repo (or the current master until we reach a distributable beta), you should use the devel branch of the vcglib. 

### Compiling
MeshLab has a plugin architecture and therefore all the plugins are compiled separately; some of them are harder to be compiled. Don't worry, if a plugin fails to compile just remove it and you lose just that functionality. As a first step you should try to use the `src/meshlab_mini.pro` that contains the info for building meshlab with a minimal set of plugins with no external dependencies and usually compile with no problem.

Some plugins of MeshLab need external libraries. All the required libraries are included in the `meshlab/src/external` folder. You have to compile these libraries before attempting to compile the whole MeshLab. Just use the `meshlab/src/external.pro` file. For OsX 10.12 we kindly provide the already compiled binaries. 

Once you have the required lib (check for lib files in the folder `meshlab/src/external/lib/<your_architecture>`) you can try to compile the whole meshlab using `src/meshlab_full.pro`. 

### Platform specific notes
On __osx__ some plugins exploit openmp parallelism (screened poisson, isoparametrization) so you need a compiler supporting it and the clang provided by xcode does not support openmp. Install 'clang++-mp-3.9' using macport or modify the .pro accordingly.

On __Linux__, you may optionally choose to use your system installs of some libraries rather than the bundled versions.
The library name, argument to add to all `qmake` calls (e.g. by adding to `QMAKE_FLAGS` in the example below), and Debian package name are listed below:

* Eigen3 (at least 3.2) - `CONFIG+=system_eigen3` - package `libeigen3-dev`
* GLEW (version 2 or newer) - `CONFIG+=system_glew` - package `libglew-dev`
* lib3ds - `CONFIG+=system_lib3ds` - package `lib3ds-dev`
* OpenCTM - `CONFIG+=system_openctm` - package `libopenctm-dev`
* BZip2 - `CONFIG+=system_bzip2` - package `libbz2-dev`

The versions found in Debian Buster are all new enough to meet these requirements.

###### Ubuntu 16 Compilation example
* Make sure you selected the correct version of Qt: `qmake -v`. You can use `qtchooser -l` to list the versions and if in doubt use the direct path to your qmake binary.
* Clone repositories:
```
git clone --depth 1 git@github.com:cnr-isti-vclab/meshlab.git
git clone --depth 1 git@github.com:cnr-isti-vclab/vcglib.git -b devel
cd meshlab
```
* Set build flags:
```and
QMAKE_FLAGS=('-spec' 'linux-g++' 'CONFIG+=release' 'CONFIG+=qml_release' 'CONFIG+=c++11' 'QMAKE_CXXFLAGS+=-fPIC' 'QMAKE_CXXFLAGS+=-std=c++11' 'QMAKE_CXXFLAGS+=-fpermissive' 'INCLUDEPATH+=/usr/include/eigen3' "LIBS+=-L`pwd`/lib/linux-g++")
MAKE_FLAGS=('-j11')
```
* Build:
```
cd src/external
qmake external.pro ${QMAKE_FLAGS[@]} && make $MAKE_FLAGS
cd ../common
qmake common.pro ${QMAKE_FLAGS[@]} && make $MAKE_FLAGS
cd ..
qmake meshlab_mini.pro ${QMAKE_FLAGS[@]} && make $MAKE_FLAGS
qmake meshlab_full.pro ${QMAKE_FLAGS[@]} && make $MAKE_FLAGS
```
* Run:
```
./distrib/meshlab
```
