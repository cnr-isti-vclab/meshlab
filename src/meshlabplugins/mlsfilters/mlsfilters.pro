include(../../shared.pri)

# CONFIG += debug

HEADERS = rimls.h mlsplugin.h

SOURCES =  balltree.cpp kdtree.cpp mlssurface.cpp rimls.cpp apss.cpp mlsplugin.cpp \
	../../meshlab/filterparameter.cpp \
	$$GLEWCODE

TARGET = mlsfilters

CONFIG       += opengl

