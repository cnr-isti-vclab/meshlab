include (../../shared.pri)

HEADERS += \
    ioraster_base.h

SOURCES += \
    ioraster_base.cpp \
    $$MESHLAB_EXTERNAL_DIRECTORY/easyexif/exif.cpp

INCLUDEPATH += $$MESHLAB_EXTERNAL_DIRECTORY/easyexif

TARGET = ioraster_base
