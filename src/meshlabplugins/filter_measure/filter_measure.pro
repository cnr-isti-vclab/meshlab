include (../../shared.pri)

HEADERS       += filter_measure.h
SOURCES       += filter_measure.cpp 
TARGET         = filter_measure
PRE_TARGETDEPS += filter_measure.xml

macx:QMAKE_POST_LINK = "cp "$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml"

#INSTALLS += sample_xmlfilter.xml



