include (../general.pri)

INCLUDEPATH += . .. $$VCGDIR $$EIGENDIR $$GLEWDIR/include

HEADERS        = 

SOURCES        = mainserver.cpp \

RESOURCES = ../meshlab/meshlab.qrc \
			meshlabserver.qrc

# to add windows icon 
RC_FILE = ../meshlab/meshlab.rc

QT += xml opengl
QT += xmlpatterns
QT += script
#QT -= gui # Only the core module is used.
DESTDIR = ../distrib
macx:DESTDIR = ../distrib/meshlab.app/Contents/MacOS/
macx:QMAKE_POST_LINK ="install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib ../distrib/meshlab.app/Contents/MacOS/meshlabserver"

mac:LIBS += ../distrib/meshlab.app/Contents/MacOS/libcommon.dylib

win32-msvc:  LIBS += -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2005:  LIBS += -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2008:  LIBS += -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2010:  LIBS += -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2012:  LIBS += -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2015:  LIBS += -L../distrib -lcommon -lopengl32 -lGLU32
win32-g++:  LIBS += -L../distrib -lcommon      
linux:  LIBS += -L../distrib -lcommon
linux:QMAKE_RPATHDIR += ../distrib
#CONFIG (release,debug | release) {
#	win32-msvc2005:release:  LIBS += -L../common/release -lcommon
#	win32-msvc2008:release:  LIBS += -L../common/release -lcommon
#	win32-g++:release:LIBS += -L../common/release -lcommon
#}

win32-msvc:DEFINES += GLEW_STATIC
win32-msvc2005:DEFINES += GLEW_STATIC
win32-msvc2008:DEFINES += GLEW_STATIC
win32-msvc2010:DEFINES += GLEW_STATIC 
win32-msvc2012:DEFINES += GLEW_STATIC
win32-msvc2015:DEFINES += GLEW_STATIC

#DEFINES += GLEW_STATIC

# This removes the need of XSERVER to run meshlabserver
DEFINES += NO_XSERVER_DEPENDENCY

CONFIG += stl 
CONFIG += console

# Mac specific Config required to avoid to make application bundles
CONFIG -= app_bundle
