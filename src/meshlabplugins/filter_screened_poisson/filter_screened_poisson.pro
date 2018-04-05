include (../../shared.pri)

# uncomment these three lines for using the latest clang compiler on OSX to use openmp
# using macports install clang-6.0 and libomp
macx:QMAKE_CXX = /opt/local/bin/clang++-mp-6.0
macx:QMAKE_CXXFLAGS += -fopenmp
macx:QMAKE_LFLAGS += -L/opt/local/lib/libomp -lomp

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
