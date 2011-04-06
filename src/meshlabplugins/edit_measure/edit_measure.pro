include (../../shared.pri)

HEADERS   = edit_measure_factory.h \
            edit_measure.h

SOURCES   = edit_measure_factory.cpp \
            edit_measure.cpp \
            $$VCGDIR/wrap/gui/rubberband.cpp

TARGET    = edit_measure

QT       += opengl

RESOURCES = edit_measure.qrc
