# this is the common include for anything compiled inside the meshlab pro
# contains general preprocesser, compiler and linker settings,
# paths for dependecies and so on

######## GENERAL SETTINGS ##########

# This is the main coord type inside meshlab
# it can be double or float according to user needs.
DEFINES += MESHLAB_SCALAR=float

# VCG directory
VCGDIR = $$MESHLAB_SOURCE_DIRECTORY/../vcglib

# MeshLab requires C++11
CONFIG += c++11

# MeshLab requires OpenMP
QMAKE_CXXFLAGS+= -fopenmp

#Debug and Release configs
CONFIG(release, debug|release):QMAKE_CXXFLAGS += -O3 -DNDEBUG
CONFIG(debug, debug|release):QMAKE_CXXFLAGS += -O0 -g

#Eigen and glew dirs
CONFIG(system_eigen3): EIGENDIR = /usr/include/eigen3
!CONFIG(system_eigen3):EIGENDIR = $$VCGDIR/eigenlib
!CONFIG(system_glew):  GLEWDIR = $$MESHLAB_EXTERNAL_DIRECTORY/glew-2.1.0


######## WINDOWS SETTINGS ##########

# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

# Set up library search paths
win32-msvc:QMAKE_LFLAGS+= -L$$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc -L$$MESHLAB_DISTRIB_DIRECTORY/lib
win32-gcc:QMAKE_LFLAGS+= -L$$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc -L$$MESHLAB_DISTRIB_DIRECTORY/lib


######## MACOS SETTINGS ##########

macx:QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override

# using brew install clang from llvm and libomp
# uncomment these three lines for using default OSX compiler
macx:QMAKE_CXX = /usr/local/opt/llvm/bin/clang++
macx:QMAKE_CXXFLAGS += -I/usr/local/opt/llvm/include
macx:QMAKE_LFLAGS += -L/usr/local/opt/llvm/lib

# Other flags for OpenMP
macx:QMAKE_CXXFLAGS += -Xpreprocessor
macx:QMAKE_LFLAGS += -lomp

# Set up library search paths
macx:QMAKE_LFLAGS+= -L$$MESHLAB_DISTRIB_DIRECTORY/lib/macx64 -L$$MESHLAB_DISTRIB_DIRECTORY/lib


######## LINUX SETTINGS ##########

#linux-g++:QMAKE_CXXFLAGS+=-Wno-unknown-pragmas

# Set up library search paths
linux:QMAKE_RPATHDIR += $$MESHLAB_DISTRIB_DIRECTORY/lib
linux:QMAKE_LFLAGS+= -L$$MESHLAB_DISTRIB_DIRECTORY/lib/linux -L$$MESHLAB_DISTRIB_DIRECTORY/lib
linux:QMAKE_LFLAGS+= -L$$MESHLAB_DISTRIB_DIRECTORY/lib/linux -L$$MESHLAB_DISTRIB_DIRECTORY/lib
