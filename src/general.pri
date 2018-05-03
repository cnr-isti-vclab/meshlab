# this is the common include for anything compiled inside the meshlab pro


# This is the main coord type inside meshlab
# it can be double or float according to user needs.
DEFINES += MESHLAB_SCALAR=float

VCGDIR   = ../../../vcglib
EIGENDIR = $$VCGDIR/eigenlib
GLEWDIR = ../external/glew-2.1.0

CONFIG += c++11

macx:QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override
macx:CONFIG(release, debug|release):QMAKE_CXXFLAGS += -O3 -DNDEBUG
macx:CONFIG(debug, debug|release):QMAKE_CXXFLAGS += -O0 -g

# uncomment these three lines for using the latest clang compiler on OSX to use openmp
# using macports install clang-6.0 and libomp
macx:QMAKE_CXX = /opt/local/bin/clang++-mp-6.0
macx:QMAKE_CXXFLAGS += -fopenmp
macx:QMAKE_LFLAGS += -L/opt/local/lib/libomp -lomp


MACLIBDIR = ../../external/lib/macx64

# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
linux-g++:QMAKE_CXXFLAGS+=-Wno-unknown-pragmas
