include (../../shared.pri)

HEADERS += \
    filter_mutualinfo_xml.h \
    alignset.h \
    levmarmethods.h \
    mutual.h \
    parameters.h \
    pointCorrespondence.h \
    pointOnLayer.h \
    shutils.h \
    solver.h

SOURCES += \
    filter_mutualinfo_xml.cpp \
    alignset.cpp  \
    levmarmethods.cpp  \
    mutual.cpp  \
    parameters.cpp   \
    pointCorrespondence.cpp  \
    solver.cpp
		
TARGET = filter_mutualinfo_xml

INCLUDEPATH *= \
    $$MESHLAB_EXTERNAL_DIRECTORY/levmar-2.3 \
    $$MESHLAB_EXTERNAL_DIRECTORY/newuoa/include

include (../../shared_post.pri)
