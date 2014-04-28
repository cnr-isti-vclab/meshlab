include (../../shared.pri)

HEADERS       += sample_xmlfilter.h

SOURCES       += sample_xmlfilter.cpp \

TARGET        = sample_xmlfilter

#INSTALLS += sample_xmlfilter.xml

PRE_TARGETDEPS += sample_xmlfilter.xml
macx:QMAKE_POST_LINK = "rsync -u "$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml; rsync -u ../../distrib/plugins/"$$TARGET".xml "$$TARGET".xml"
