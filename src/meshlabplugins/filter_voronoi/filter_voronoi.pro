include (../../shared.pri)

HEADERS       += filter_voronoi.h

SOURCES       += filter_voronoi.cpp

TARGET        = filter_voronoi
PRE_TARGETDEPS += filter_voronoi.xml

macx:QMAKE_POST_LINK = "rsync -u "$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml; rsync -u ../../distrib/plugins/"$$TARGET".xml "$$TARGET".xml"
