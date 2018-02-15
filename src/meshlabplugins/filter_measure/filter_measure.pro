include (../../shared.pri)

HEADERS       += filter_measure.h
SOURCES       += filter_measure.cpp 
TARGET         = filter_measure
#PRE_TARGETDEPS += filter_measure.xml

QMAKE_POST_LINK += $$QMAKE_COPY_FILE \"$${_PRO_FILE_PWD_}/$${TARGET}.xml\" \"../../distrib/plugins/$${TARGET}.xml\";
