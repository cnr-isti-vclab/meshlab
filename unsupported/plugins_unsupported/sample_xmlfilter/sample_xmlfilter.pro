include (../../shared.pri)

HEADERS       += sample_xmlfilter.h

SOURCES       += sample_xmlfilter.cpp \

TARGET        = sample_xmlfilter

#INSTALLS += sample_xmlfilter.xml

PRE_TARGETDEPS += sample_xmlfilter.xml
macx:QMAKE_POST_LINK = "\
    rsync -u "$$TARGET".xml $$MESHLAB_DISTRIB_DIRECTORY/plugins/"$$TARGET".xml; \
    rsync -u $$MESHLAB_DISTRIB_DIRECTORYb/plugins/"$$TARGET".xml "$$TARGET".xml \
"
