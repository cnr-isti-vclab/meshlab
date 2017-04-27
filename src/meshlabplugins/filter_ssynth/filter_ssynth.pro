include (../../shared.pri)
HEADERS += filter_ssynth.h \
    ../../meshlabplugins/io_x3d/import_x3d.h \
    ../../meshlabplugins/io_x3d/util_x3d.h \
    ../../meshlabplugins/io_x3d/vrml/Parser.h \
    ../../meshlabplugins/io_x3d/vrml/Scanner.h \
    mytrenderer.h
SOURCES += filter_ssynth.cpp \
    ../../meshlabplugins/io_x3d/vrml/Parser.cpp \
    ../../meshlabplugins/io_x3d/vrml/Scanner.cpp \
    mytrenderer.cpp
INCLUDEPATH += ../../external/structuresynth-1.5/ssynth
win32-msvc.net:LIBS += ../../external/lib/win32-msvc.net/ssynth.lib
win32-msvc2005:LIBS += ../../external/lib/win32-msvc2005/ssynth.lib
win32-msvc2008:LIBS += ../../external/lib/win32-msvc2008/ssynth.lib
win32-msvc2010:LIBS += ../../external/lib/win32-msvc2010/ssynth.lib
win32-msvc2012:LIBS += ../../external/lib/win32-msvc2012/ssynth.lib
win32-msvc2015:LIBS += ../../external/lib/win32-msvc2015/ssynth.lib
win32-g++:LIBS += ../../external/lib/win32-gcc/libssynth.a
linux-g++:LIBS += $$PWD/../../external/lib/linux-g++/libssynth.a
macx:LIBS += ../../external/lib/macx64/libssynth.a
TARGET = filter_ssynth
TEMPLATE = lib

CONFIG += plugin
OTHER_FILES += x3d.rendertemplate \
    x3d2.rendertemplate \
    x3d3.rendertemplate \
    x3d4.rendertemplate
RESOURCES += ssynthres.qrc
