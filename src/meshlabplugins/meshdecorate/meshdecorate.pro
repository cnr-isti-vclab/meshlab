include (../../sharedfilter.pri)

QT += opengl

HEADERS       += meshdecorate.h
SOURCES       += meshdecorate.cpp \
                ../../common/pluginmanager.cpp \
                $$GLEWCODE\
				$$VCGDIR/wrap/gui/coordinateframe.cpp

TARGET        = meshdecorate
