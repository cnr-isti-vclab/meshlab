include (../../shared.pri)

HEADERS       = edit_measure_factory.h \
				editmeasure.h 

SOURCES       = edit_measure_factory.cpp \
				editmeasure.cpp \
				$$VCGDIR/wrap/gui/rubberband.cpp
				
TARGET        = editmeasure

QT           += opengl

RESOURCES     = editmeasure.qrc
