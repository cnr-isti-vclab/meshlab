include (../../shared.pri)

HEADERS += \
    edit_paint_factory.h \
    cloneview.h \
    colorframe.h \
    paintbox.h \
    edit_paint.h

SOURCES += \
    edit_paint_factory.cpp \
    paintbox.cpp \
    edit_paint.cpp

RESOURCES += \
    edit_paint.qrc

FORMS += \
    paintbox.ui

TARGET = edit_paint

