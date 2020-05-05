include (../../shared.pri)

win32:QMAKE_CXXFLAGS   += -openmp
macx:QMAKE_CXXFLAGS_RELEASE+= -O3 -DRELEASE -funroll-loops -ffast-math  -Wno-sign-compare -Wno-unused-parameter
linux:QMAKE_LFLAGS += -fopenmp -lgomp

HEADERS += \
    filter_screened_poisson.h \
    poisson_utils.h

SOURCES += \
    filter_screened_poisson.cpp \
    Src/MarchingCubes.cpp \
    Src/Factor.cpp \
    Src/Geometry.cpp


TARGET = filter_screened_poisson
DEFINES += BRUNO_LEVY_FIX 
DEFINES += FOR_RELEASE

