include (../../shared.pri)

# Used by the trackball
VCGDIR = ../../../../vcglib
HEADERS += edit_scan.h
HEADERS += widget.h

HEADERS += $$VCGDIR/wrap/gui/trackball.h \
    $$VCGDIR/wrap/gui/trackmode.h
SOURCES += edit_scan.cpp # widget.cpp
SOURCES += $$VCGDIR/wrap/gui/trackball.cpp \
    $$VCGDIR/wrap/gui/trackmode.cpp
TARGET = edit_scan
QT += opengl
RESOURCES = meshlab.qrc
FORMS += widget.ui
