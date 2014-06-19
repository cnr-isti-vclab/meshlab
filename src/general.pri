# this is the common include for anything compiled inside the meshlab pro


# This is the main coord type inside meshlab
# it can be double or float according to user needs.
DEFINES += MESHLAB_SCALAR=float

VCGDIR = ../../../vcglib
GLEWDIR = ../external/glew-1.7.0

#mac:QMAKE_CXX = g++-4.2
macx:QMAKE_CXX=clang++
macx:QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
macx:QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
#macx:QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++

# uncomment to try Eigen
# DEFINES += VCG_USE_EIGEN
# CONFIG += warn_off


#macx:QMAKE_CXX=g++-4.2
macx: {
  contains(QMAKE_CXX,clang++) {
    MACLIBDIR = ../../external/lib/macx64
    } else {
    MACLIBDIR = ../../external/lib/macx32
  }
}

# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
