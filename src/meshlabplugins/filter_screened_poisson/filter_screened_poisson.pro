include (../../shared.pri)
#QMAKE_CXXFLAGS += -Wno-sign-compare -Wno-unused-parameter
#QMAKE_CXXFLAGS -= -Wall -W
macx:QMAKE_CXX = clang++-mp-3.9
macx:QMAKE_LFLAGS += -L/opt/local/lib/libomp -lomp
QMAKE_CXXFLAGS+=-fopenmp

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


#PRE_TARGETDEPS += ./filter_screened_poisson.xml
macx:QMAKE_POST_LINK = "rsync -u "$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml; rsync -u ../../distrib/plugins/"$$TARGET".xml "$$TARGET".xml"

