include (../../shared.pri)

HEADERS       += filter_screened_poisson.h

SOURCES       += filter_screened_poisson.cpp \
                  Src/MarchingCubes.cpp \
                Src/PlyFile.cpp \
                Src/CmdLineParser.cpp \
                Src/Factor.cpp \
                Src/Geometry.cpp \
                Src/Time.cpp

TARGET        = filter_screened_poisson

PRE_TARGETDEPS += filter_screened_poisson.xml
macx:QMAKE_POST_LINK = "rsync -u "$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml; rsync -u ../../distrib/plugins/"$$TARGET".xml "$$TARGET".xml"
