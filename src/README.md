# MeshLab Source Code structure

In the `src` directory there are several folders containing all the source code and configuration files that allows to build MeshLab.

The source code of MeshLab is structured in the following directories:

 * cmake: it contains a series of cmake scripts used to find external libraries;
 * [external](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/external): it contains a series of external libraries needed by several plugins. Some of these libraries are compiled before the compilation of meshlab, if a corresponding system library is not found and then linked; other are header-only libraries that are just included;
 * [common](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/common): a series of utility classes and functions used by MeshLab and its plugins;
 * [meshlab](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/meshlab): GUI and core of MeshLab;
 * [meshlabserver](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/meshlabserver): a tool that allows to compute mesh operations through command line;
 * [meshlabplugins](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/meshlabplugins): all the plugins that can be added to MeshLab;
 * [use_cpu_opengl](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/use_cpu_opengl): a tool compiled only under windows that allows to use non-GPU accelerated OpenGL calls;
 * [vcglib](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/vcglib): submodule containing the vcglib.

## Build MeshLab

MeshLab builds with the three major compilers: `gcc`, `clang`, and `msvc`. It requires [Qt](https://www.qt.io/) >= 5.12.

After setting up the Qt environment:

	git clone --recursive https://github.com/cnr-isti-vclab/meshlab
	mkdir meshlab/src/build
	cd meshlab/src/build
    cmake ..
    make


You can also use [QtCreator](https://www.qt.io/product) to build meshlab:

1. Install QtCreator and Qt >= 5.12;
2. Open `CMakeLists.txt` inside `src`;
3. Select your favourite shadow build directory;
4. Build meshlab.

MeshLab has a plugin architecture and therefore all the plugins are compiled separately; some of them are harder to be compiled. Don't worry: if a plugin fails to compile, just remove it and you lose just that functionality.

### Platform specific notes
On __osx__ some plugins exploit openmp parallelism (screened poisson, isoparametrization) so you need a compiler supporting it and the clang provided by xcode does not support openmp. You can install all the required libraries by running the following command in a terminal:

	brew install llvm libomp

On __Windows__, we suggest to build meshlab using QtCreator. Before trying to build, you should:

 * install VisualStudio >= 2017 with the C++ developement package;
 * install Qt >= 5.12 and QtCreator.

then, open the CMakeLists.txt file and try to build MeshLab.


Qt5_DIR: C:/Qt/Qt5.12.10/5.12.10/msvc2017_64/lib/cmake/Qt5
