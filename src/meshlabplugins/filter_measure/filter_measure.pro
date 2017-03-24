include (../../shared.pri)

HEADERS       += filter_measure.h
SOURCES       += filter_measure.cpp 
TARGET         = filter_measure
#PRE_TARGETDEPS += filter_measure.xml

macx:QMAKE_POST_LINK = "cp "$$_PRO_FILE_PWD_/$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml"
linux:QMAKE_POST_LINK = "cp "$$_PRO_FILE_PWD_/$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml; cd ../../distrib/plugins/ ; ln -s "$$TARGET".xml lib"$$TARGET".xml"
