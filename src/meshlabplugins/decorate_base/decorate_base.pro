include (../../shared.pri)

HEADERS += decorate_base.h \
    colorhistogram.h \
 $$VCGDIR/wrap/qt/gl_label.h
SOURCES += decorate_base.cpp \
    $$VCGDIR/wrap/gui/coordinateframe.cpp
TARGET = decorate_base
