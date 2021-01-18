include (../../shared.pri)

QT += qml

HEADERS += \
    filter_sketchfab.h \
    miniz.h

SOURCES += \
    filter_sketchfab.cpp \
    $$VCGDIR//wrap/ply/plylib.cpp

TARGET = filter_sketchfab
