include (../../shared.pri)

HEADERS       += filter_poisson.h

QMAKE_CXXFLAGS += -fpermissive

SOURCES += src/Factor.cpp \
           src/Geometry.cpp \
           src/MarchingCubes.cpp \
           src/MultiGridOctest.cpp \
           filter_poisson.cpp 

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
CONFIG       += opengl


