include (../../shared.pri)

QT += opengl

HEADERS       += meshdecorate.h
SOURCES       += meshdecorate.cpp \
               $$VCGDIR/wrap/gui/coordinateframe.cpp

TARGET        = meshdecorate
