include (../../shared.pri)

HEADERS       = editmeasure.h 

SOURCES       = editmeasure.cpp \
		$$GLEWCODE \
		$$VCGDIR/wrap/gui/rubberband.cpp
TARGET        = editmeasure
QT           += opengl
RESOURCES     = editmeasure.qrc
