HEADERS        = interfaces.h \
		 GLLogStream.h \
                 mainwindow.h \
                 meshmodel.h \
                 glarea.h \
                 plugindialog.h \
                 customDialog.h \
                 ../../../sf/wrap/gui/trackball.h\
                 ../../../sf/wrap/gui/trackmode.h\
                 ../../../sf/wrap/gl/trimesh.h
SOURCES        = main.cpp \
                 mainwindow.cpp \
                 meshmodel.cpp \
		 GLLogStream.cpp \
                 glarea.cpp \
                 plugindialog.cpp \
                 customDialog.cpp \
                 ../../../sf/wrap/ply/plylib.cpp\
                 ../../../sf/wrap/gui/trackball.cpp\
                 ../../../sf/wrap/gui/trackmode.cpp
RESOURCES     = meshlab.qrc
QT           += opengl
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

INCLUDEPATH += . ../../../sf ../../../code/lib/glew/include
CONFIG += stl
win32:LIBS	+= ../../../code/lib/glew/lib/glew32.lib 
unix{
	LIBS		+= -lGLEW
	QMAKE_CC	 = gcc-3.3
	QMAKE_CXX	 = g++-3.3
	QMAKE_LINK	 = gcc-3.3
	CONFIG		+= debug
}
# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaint
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS plugandpaint.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaint
INSTALLS += target sources
