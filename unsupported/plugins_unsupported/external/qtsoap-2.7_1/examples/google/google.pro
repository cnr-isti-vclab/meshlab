TEMPLATE = app
INCLUDEPATH += .

include(../../src/qtsoap.pri)

# Input
HEADERS += google.h
SOURCES += google.cpp main.cpp
