include (../../shared.pri)

HEADERS += \
    filter_qhull.h \
    qhull_tools.h

SOURCES += \
    filter_qhull.cpp \
    qhull_tools.cpp

system_qhull {
    INCLUDEPATH += /usr/include/libqhull
    DEFINES += SYSTEM_QHULL
}
!system_qhull {
INCLUDEPATH += \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src
}

win32-msvc:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/qhull.lib
win32-g++:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/libqhull.a
macx:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/libqhull.a
linux:LIBS += -lqhull

TARGET = filter_qhull
