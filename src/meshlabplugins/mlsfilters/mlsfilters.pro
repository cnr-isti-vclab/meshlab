include(../../shared.pri)

# CONFIG += debug

HEADERS = rimls.h mlsplugin.h \
			../../meshlab/meshmodel.h

SOURCES =  balltree.cpp kdtree.cpp mlssurface.cpp rimls.cpp apss.cpp mlsplugin.cpp \
	../../meshlab/filterparameter.cpp \
	../../meshlab/meshmodel.cpp \
	$$GLEWCODE

TARGET = mlsfilters

CONFIG       += opengl

