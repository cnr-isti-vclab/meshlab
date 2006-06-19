HEADERS        = ../meshlab/interfaces.h \
		         ../meshlab/GLLogStream.h \
                 ../meshlab/mainwindow.h \
                 ../meshlab/meshmodel.h \
                 ../meshlab/glarea.h \
                 ../meshlab/filterscript.h \
                 ../meshlab/filterparameter.h \
                 ../meshlab/plugindialog.h \
                 ../meshlab/customDialog.h \
                 ../meshlab/filterScriptDialog.h \
                 ../meshlab/saveSnapshotDialog.h \
                 ../meshlab/savemaskexporter.h \
                 ../meshlab/changetexturename.h \
                 ../meshlab/GenericELDialog.h \
                 ../../../sf/wrap/gui/trackball.h\
                 ../../../sf/wrap/gui/trackmode.h\
                 ../../../sf/wrap/gl/trimesh.h
SOURCES        = mainserver.cpp \
                 ../meshlab/mainwindow_Init.cpp \
                 ../meshlab/mainwindow_RunTime.cpp\
                 ../meshlab/meshmodel.cpp \
		         ../meshlab/GLLogStream.cpp \
                 ../meshlab/glarea.cpp \
                 ../meshlab/plugindialog.cpp \
                 ../meshlab/filterscript.cpp \
                 ../meshlab/customDialog.cpp \
                 ../meshlab/filterScriptDialog.cpp \
                 ../meshlab/saveSnapshotDialog.cpp \
                 ../meshlab/savemaskexporter.cpp \
                 ../meshlab/changetexturename.cpp \
                 ../../../sf/wrap/gui/trackball.cpp\
                 ../../../sf/wrap/gui/trackmode.cpp \
                 ../../../code/lib/glew/src/glew.c

FORMS          =	../meshlab/ui/filterScriptDialog.ui \
                    ../meshlab/ui/customDialog.ui\
					../meshlab/ui/savesnapshotDialog.ui\						 
					../meshlab/ui/aboutDialog.ui \
					../meshlab/ui/renametexture.ui \
					../meshlab/ui/savemaskexporter.ui \
					../meshlab/ui/GenericELDialog.ui


RESOURCES     = ../meshlab/meshlab.qrc

# to add windows icon 
RC_FILE = ../meshlab/meshlab.rc

QT           += opengl 
QT           += xml

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

DEFINES += GLEW_STATIC

INCLUDEPATH += . ../../../sf ../../../code/lib/glew/include
CONFIG += stl 
CONFIG += console
#win32:LIBS	+= ../../../code/lib/glew/lib/glew32.lib 
win32-g++{
#LIBS	+= glew32
#LIBPATH += ../../../code/lib/glew/lib/
}

unix{
	LIBS		+= -lGLEW
	QMAKE_CC	 = gcc
	QMAKE_CXX	 = g++
	QMAKE_LINK	 = gcc
	CONFIG		+= warn_off debug_and_release
}
