include(../../shared.pri)

# CONFIG += debug

HEADERS =	apss.h\ 
		mlsmarchingcube.h \		
		priorityqueue.h \
		balltree.h \			
		mlsplugin.h \			
		rimls.h \
		implicits.h \			
		mlssurface.h \			
		smallcomponentselection.h \
		kdtree.h \			
		mlsutils.h \	
		../../meshlab/meshmodel.h

SOURCES =	apss.cpp \	
		balltree.cpp \	
		mlsplugin.cpp \		
		rimls.cpp \
		apss.tpp \		
		kdtree.cpp \	
		mlssurface.tpp \
		rimls.tpp \	
		../../meshlab/filterparameter.cpp \
		../../meshlab/meshmodel.cpp \
	$$GLEWCODE

TARGET = filter_mls

CONFIG       += opengl
CONFIG       += warn_off

