include (../../shared.pri)

macx:QMAKE_CXX = clang++-mp-3.9
macx:QMAKE_LFLAGS += -L/opt/local/lib/libomp -lomp
linux:QMAKE_LFLAGS += -fopenmp -lgomp
macx:QMAKE_CXXFLAGS_RELEASE+= -O3 -DRELEASE -funroll-loops -ffast-math  -Wno-sign-compare -Wno-unused-parameter

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
macx:QMAKE_POST_LINK = "cp "$$_PRO_FILE_PWD_/$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml"
linux:QMAKE_POST_LINK = "cp "$$_PRO_FILE_PWD_/$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml; cd ../../distrib/plugins/ ; ln -s "$$TARGET".xml lib"$$TARGET".xml"
