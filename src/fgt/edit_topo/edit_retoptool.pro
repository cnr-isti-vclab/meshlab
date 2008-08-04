include (../../shared.pri)

FORMS 			+= 	edit_retoptooldialog.ui

HEADERS       	= 	edit_retoptool.h \
					edit_retoptooldialog.h \
					edit_retoptoolmeshbuilder.h

SOURCES      	= 	edit_retoptool.cpp \
					edit_retoptooldialog.cpp \
					edit_retoptoolmeshbuilder.cpp \
					$$GLEWCODE 
				
TARGET        	= 	edit_retoptool

QT           	+= 	opengl

RESOURCES     	= 	edit_retoptool.qrc

