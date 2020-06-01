# MeshLab Source Code structure

In the `src` folder there are several folders containing all the source code and configuration files that allows to compile MeshLab.

The source code of MeshLab is structured in the following folders:

 * [external](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/external): it contains a series of external libraries needed by several plugins. Some of these libraries are compiled before the compilation of meshlab, if a corresponding systme library is not found and then linked; some other libraries are just included by some plugins.
 * [common](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/common): a series of utility functions used by MeshLab and its plugins.
 * [meshlab](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/meshlab): GUI and core of MeshLab.
 * [meshlabserver](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/meshlabserver): a tool that allows to compute mesh operations through command line
 * [meshlabplugins](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/meshlabplugins): all the plugins that can be added to MeshLab.
 
The following folders are used by `cmake`:
 
 * cmake;
 * templates.
 
There are also two folders that contains MeshLab plugins that are no longer supported or are experimental, and these plugins are not compiled in any of MeshLab configurations:
 
 * plugins_experimental;
 * plugins_unsupported.
 
## Compiling MeshLab

MeshLab compiles with the three major compilers: `gcc`, `clang`, and `msvc`.

MeshLab requires [Qt](https://www.qt.io/) >= 5.9, with `xmlpatterns` as additional package.

After setting up the Qt environment:

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	cd meshlab/src
	qmake
	make
	
Then, a meshlab executable can be found inside the `distrib` folder.

You can also use `cmake`:

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	cd meshlab
	cmake src/
	make

You can also use [QtCreator](https://www.qt.io/product) to build meshlab:

1. Install QtCreator and Qt >= 5.9 with `xmlpatterns` as additional package;
2. Open `meshlab.pro` inside `src`;
3. Select your favourite shadow build directory;
4. Before the build, deactive the `QtQuickCompiler` option from the qmake call in the project options;
5. Build meshlab.

MeshLab has a plugin architecture and therefore all the plugins are compiled separately; some of them are harder to be compiled. Don't worry, if a plugin fails to compile just remove it and you lose just that functionality. As a first step you should try to compile MeshLab with the configuration "meshlab_mini":

	qmake "CONFIG+=meshlab_mini"
	make	
	
This configuration contains the info for building meshlab with a minimal set of plugins.

Some plugins of MeshLab need external libraries. All the required libraries are included in the `meshlab/src/external` folder, that are automatically compiled before MeshLab in all its configurations.

### Platform specific notes
On __osx__ some plugins exploit openmp parallelism (screened poisson, isoparametrization) so you need a compiler supporting it and the clang provided by xcode does not support openmp. Qmake is configured in order to look for the `clang` compiler that is automatically installed by `homebrew`, calling:

	brew install llvm libomp

On __Linux__, you may optionally choose to use your system installs of some libraries rather than the bundled versions.
The library name, argument to add to all `qmake` calls (e.g. by adding to `QMAKE_FLAGS` in the example below), and Debian package name are listed below:

* Eigen3 (at least 3.2) - `CONFIG+=system_eigen3` - package `libeigen3-dev`
* GLEW (version 2 or newer) - `CONFIG+=system_glew` - package `libglew-dev`
* lib3ds - `CONFIG+=system_lib3ds` - package `lib3ds-dev`
* OpenCTM - `CONFIG+=system_openctm` - package `libopenctm-dev`
* BZip2 - `CONFIG+=system_bzip2` - package `libbz2-dev`

The versions found in Debian Buster are all new enough to meet these requirements.
