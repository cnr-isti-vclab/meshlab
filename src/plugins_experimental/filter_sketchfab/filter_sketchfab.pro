include (../../shared.pri)

HEADERS += filter_sketchfab.h
SOURCES += filter_sketchfab.cpp
TARGET   = filter_sketchfab
PRE_TARGETDEPS += filter_sketchfab.xml
macx:QMAKE_POST_LINK = "cp "$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml"

