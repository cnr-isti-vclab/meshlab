HEADERS        = ../meshlab/interfaces.h \
                 ../meshlab/meshmodel.h \
                 ../meshlab/filterscript.h \
                 ../meshlab/filterparameter.h 

SOURCES        = mainserver.cpp \
                 ../meshlab/filterscript.cpp \
                 ../meshlab/plugin_support.cpp 


# to add windows icon 
RC_FILE = ../meshlab/meshlab.rc

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
