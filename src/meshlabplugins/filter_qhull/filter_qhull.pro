include (../../shared.pri)

HEADERS += \
    filter_qhull.h \
    qhull_tools.h

SOURCES += \
    filter_qhull.cpp \
    qhull_tools.cpp

INCLUDEPATH += \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src

win32-msvc:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc/qhull.lib
win32-g++:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc/libqhull.a
macx:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/macx64/libqhull.a
linux-g++:LIBS += -L$$MESHLAB_DISTRIB_DIRECTORY/lib/linux-g++ -lqhull

TARGET = filter_qhull
