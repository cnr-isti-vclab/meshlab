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

win32-msvc:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/ssynth.lib
win32-g++:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/libssynth.a
macx:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/libssynth.a
linux:LIBS += -L$$MESHLAB_DISTRIB_EXT_DIRECTORY -lssynth

TARGET = filter_ssynth


