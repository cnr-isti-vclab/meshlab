include (../../shared.pri)

HEADERS += filter_sketchfab.h
SOURCES += filter_sketchfab.cpp $$VCGDIR//wrap/ply/plylib.cpp
TARGET   = filter_sketchfab
#PRE_TARGETDEPS += filter_sketchfab.xml
macx:QMAKE_POST_LINK = "cp "$$_PRO_FILE_PWD_/$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml"

