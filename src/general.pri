# this is the common include for anything compiled inside the meshlab pro


# This is the main coord type inside meshlab
# it can be double or float according to user needs.
DEFINES += MESHLAB_SCALAR=float

VCGDIR = ../../../vcglib
GLEWDIR = ../external/glew-1.7.0

CONFIG += c++11

QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override

MACLIBDIR = ../../external/lib/macx64

# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
