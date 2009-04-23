include (../../shared.pri)

QT += opengl

HEADERS       = meshdecorate.h
SOURCES       = meshdecorate.cpp \
				$$GLEWCODE\
				$$VCGDIR/wrap/gui/coordinateframe.cpp

TARGET        = meshdecorate
