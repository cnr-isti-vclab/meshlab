include (../../shared.pri)

HEADERS       += sample_xmlfilter.h

SOURCES       += sample_xmlfilter.cpp \ 
		
TARGET        = sample_xmlfilter

#INSTALLS += sample_xmlfilter.xml

macx:QMAKE_POST_LINK = "cp "$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml"
