include (../../shared.pri)

HEADERS += \
	globalregistration.h
SOURCES += \
	globalregistration.cpp

TARGET = filter_globalregistration

CONFIG += create_prl
CONFIG += link_prl

INCLUDEPATH += $$MESHLAB_EXTERNAL_DIRECTORY/OpenGR/src
