CONFIG      += designer plugin debug_and_release
TEMPLATE    = lib
DESTDIR     = $$[QT_INSTALL_PREFIX]/plugins/designer
message(Plugin installato in $$DESTDIR)
#DESTDIR     = /home/mario/apps/qt-4.1.0-final/plugins/designer

CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,_debug)
    else: TARGET = $$join(TARGET,,d)
}

HEADERS     = ../qedgelenght/QEdgeLength.h \
qedgelengthplugin.h
SOURCES     = ../qedgelenght/QEdgeLength.cpp \
qedgelengthplugin.cpp
