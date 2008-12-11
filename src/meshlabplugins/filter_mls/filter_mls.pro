include(../../shared.pri)

# CONFIG += debug

HEADERS = rimls.h mlsplugin.h \
			../../meshlab/meshmodel.h

SOURCES =  balltree.cpp kdtree.cpp mlsplugin.cpp \
	apss.cpp rimls.cpp \
	../../meshlab/filterparameter.cpp \
	../../meshlab/meshmodel.cpp \
	$$GLEWCODE

TARGET = mlsfilters

CONFIG       += opengl
CONFIG       += warn_off

