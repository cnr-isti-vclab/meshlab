include (../../shared.pri)
HEADERS = filter_zippering.h \
    remove_small_cc.h
SOURCES = filter_zippering.cpp \
    ../../meshlab/filterparameter.cpp
TARGET = filter_zippering
QT += opengl
