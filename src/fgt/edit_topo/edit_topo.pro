include (../../shared.pri)

FORMS 			+= 	edit_topodialog.ui

HEADERS       	= 	edit_topo.h \
					edit_topodialog.h \
					edit_topomeshbuilder.h

SOURCES      	= 	edit_topo.cpp \
					edit_topodialog.cpp \
					edit_topomeshbuilder.cpp \
					$$GLEWCODE 
				
TARGET        	= 	edit_topo

QT           	+= 	opengl

RESOURCES     	= 	edit_topo.qrc

