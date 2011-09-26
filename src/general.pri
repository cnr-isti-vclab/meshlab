# this is the common include for anything compiled inside the meshlab pro

VCGDIR = ../../../vcglib
GLEWDIR = ../external/glew-1.7.0

#mac:QMAKE_CXX = g++-4.2
macx:QMAKE_CXX=clang++

# uncomment to try Eigen
# DEFINES += VCG_USE_EIGEN
# CONFIG += warn_off


#macx:QMAKE_CXX=g++-4.2
macx:QMAKE_CXX=clang++
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
