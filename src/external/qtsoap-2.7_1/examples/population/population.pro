TEMPLATE = app
INCLUDEPATH += .
CONFIG += console

include(../../src/qtsoap.pri)

HEADERS += population.h
SOURCES += main.cpp population.cpp
