include (../../shared.pri)

HEADERS += \
    filter_plymc.h

SOURCES += \
    filter_plymc.cpp \
    $$VCGDIR/wrap/ply/plylib.cpp

TARGET = filter_plymc
