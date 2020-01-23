include (../../shared.pri)

HEADERS += decorate_base.h

SOURCES += \
    decorate_base.cpp \
    $$VCGDIR/wrap/gui/coordinateframe.cpp

TARGET = decorate_base

RESOURCES += \
    decorate_base.qrc
