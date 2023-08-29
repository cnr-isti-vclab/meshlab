# MeshLab Source Code structure

In the `src` directory there are several folders containing all the source code and configuration files that allows to build MeshLab.

The source code of MeshLab is structured in the following directories:

 * cmake: it contains a series of cmake scripts used to find external libraries;
 * [external](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/external): it contains a series of cmake configuration files that will download external libraries needed by MeshLab and several plugins;
 * [common](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/common): a series of utility classes and functions used by MeshLab and its plugins;
 * [meshlab](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/meshlab): GUI and core of MeshLab;
 * [meshlabplugins](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/meshlabplugins): all the plugins that can be added to MeshLab;
 * [use_cpu_opengl](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/use_cpu_opengl): a tool compiled only under windows that allows to use non-GPU accelerated OpenGL functions;
 * [vcglib](https://github.com/cnr-isti-vclab/meshlab/tree/master/src/vcglib): submodule containing the vcglib.

## Build MeshLab

MeshLab builds with the three major compilers: `gcc`, `clang`, and `msvc`. It requires [Qt](https://www.qt.io/) 5.15, cmake and optionally ninja (required on windows). Only 64 bit is supported.

After setting up the Qt environment:

```
git clone --recursive https://github.com/cnr-isti-vclab/meshlab
mkdir meshlab/build
cd meshlab/build
cmake ..
make
```

or, using ninja (supported also on Windows):

```
cmake -GNinja ..
ninja
```

You can also use [QtCreator](https://www.qt.io/product) to build MeshLab:

1. Install QtCreator and Qt 5.15;
2. Open `CMakeLists.txt`;
4. Configure and build MeshLab.

### External libraries

External libraries required by plugins, if not found, are automatically downloaded by `cmake` during the configuration, and placed into `src/external/downloads`.

### Platform specific notes
On __osx__ some plugins exploit openmp parallelism (screened poisson, isoparametrization) so you need a compiler supporting it and the clang provided by xcode does not support openmp. You can install all the required libraries by running the following command in a terminal:

```
brew install libomp
```

On __Windows__, we suggest to build MeshLab using QtCreator. Before trying to build, you should:

 * install VisualStudio >= 2017 with the C++ development package;
 * install Qt 5.15 and QtCreator.

then, open the CMakeLists.txt file and build MeshLab.
