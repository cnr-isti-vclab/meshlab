TEMPLATE = app
INCLUDEPATH += .
CONFIG += console

include(../../src/qtsoap.pri)

# Input
HEADERS += easter.h
SOURCES += main.cpp easter.cpp
