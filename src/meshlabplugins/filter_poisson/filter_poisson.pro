TEMPLATE      = lib
CONFIG       += plugin stl
INCLUDEPATH  +=  ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = filter_poisson.h
QMAKE_CXXFLAGS += -fpermissive

SOURCES += src/Factor.cpp \
           src/Geometry.cpp \
           src/MarchingCubes.cpp \
           src/MultiGridOctest.cpp \
           filter_poisson.cpp \ 
	   ../../meshlab/filterparameter.cpp

HEADERS += src/Allocator.h \
           src/BinaryNode.h \
           src/Factor.h \
           src/FunctionData.h \
           src/Geometry.h \
           src/Hash.h \
           src/MarchingCubes.h \
           src/MultiGridOctreeData.h \
           src/Octree.h \
           src/Polynomial.h \
           src/PPolynomial.h \
           src/SparseMatrix.h \
           src/Vector.h \
           src/Geometry.inl \
           src/Octree.inl \
           src/Vector.inl \
           src/SparseMatrix.inl \
           src/Polynomial.inl \
           src/PPolynomial.inl \
           src/FunctionData.inl \
           src/MultiGridOctreeData.inl


TARGET        = filter_poisson

DESTDIR       = ../../meshlab/plugins

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
# mac:CONFIG += x86 ppc

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}



