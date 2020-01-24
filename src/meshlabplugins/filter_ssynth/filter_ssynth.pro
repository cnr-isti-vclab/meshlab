include (../../shared.pri)

INCLUDEPATH += \
    ../io_x3d/ \
    ../io_x3d/vrml \
    $$MESHLAB_EXTERNAL_DIRECTORY/structuresynth-1.5/ssynth

HEADERS += \
    filter_ssynth.h \
    mytrenderer.h

SOURCES += \
    filter_ssynth.cpp \
    mytrenderer.cpp \
    ../io_x3d/vrml/Parser.cpp \
    ../io_x3d/vrml/Scanner.cpp

RESOURCES += \
    ssynthres.qrc

OTHER_FILES += \
    x3d.rendertemplate \
    x3d2.rendertemplate \
    x3d3.rendertemplate \
    x3d4.rendertemplate

win32-msvc:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc/ssynth.lib
win32-g++:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc/libssynth.a
macx:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/macx64/libssynth.a
linux-g++:LIBS += -lssynth

TARGET = filter_ssynth


