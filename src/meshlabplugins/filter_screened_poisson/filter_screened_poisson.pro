include (../../shared.pri)


linux:QMAKE_LFLAGS += -fopenmp -lgomp
macx:QMAKE_CXXFLAGS_RELEASE+= -O3 -DRELEASE -funroll-loops -ffast-math  -Wno-sign-compare -Wno-unused-parameter


win32:QMAKE_CXXFLAGS   += -openmp

HEADERS       += filter_screened_poisson.h

SOURCES       += filter_screened_poisson.cpp \
                  Src/MarchingCubes.cpp \
                Src/PlyFile.cpp \
#                Src/CmdLineParser.cpp \
                Src/Factor.cpp \
                Src/Geometry.cpp 

TARGET        = filter_screened_poisson
DEFINES += BRUNO_LEVY_FIX 
DEFINES += FOR_RELEASE

include (../../shared_post.pri)
