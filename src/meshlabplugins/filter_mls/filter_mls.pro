include(../../shared.pri)

# CONFIG += debug

HEADERS = rimls.h mlsplugin.h \
			../../meshlab/meshmodel.h

SOURCES =  balltree.cpp kdtree.cpp mlsplugin.cpp \
	apss.cpp rimls.cpp \
	../../meshlab/filterparameter.cpp \
	../../meshlab/meshmodel.cpp \
	$$GLEWCODE

TARGET = filter_mls

CONFIG       += opengl
CONFIG       += warn_off

