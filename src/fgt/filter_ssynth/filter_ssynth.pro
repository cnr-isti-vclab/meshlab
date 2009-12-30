include (../../shared.pri)
HEADERS += filter_ssynth.h \
            ../../meshlabplugins/io_x3d/import_x3d.h \
                ../../meshlabplugins/io_x3d/util_x3d.h \
                ../../meshlabplugins/io_x3d/vrml/Parser.h \
                ../../meshlabplugins/io_x3d/vrml/Scanner.h
SOURCES += filter_ssynth.cpp \
        ../../meshlabplugins/io_x3d/vrml/Parser.cpp \
         ../../meshlabplugins/io_x3d/vrml/Scanner.cpp
TARGET = filter_ssynth
TEMPLATE = lib
QT += opengl
CONFIG += plugin
OTHER_FILES += 
