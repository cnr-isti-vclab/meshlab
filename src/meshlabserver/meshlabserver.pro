GLEWDIR = ../external/glew-1.5.1

HEADERS        = 

SOURCES        = mainserver.cpp

# to add windows icon 
RC_FILE = ../meshlab/meshlab.rc

QT           += xml opengl

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

mac:LIBS += ../meshlab/meshlab.app/Contents/MacOS/libcommon.dylib

win32-msvc2005:debug:  LIBS += -L../common/debug -lcommon
win32-msvc2008:debug:  LIBS += -L../common/debug -lcommon
win32-g++:debug:  LIBS += -L../common/debug -lcommon

win32-msvc2005:release:  LIBS += -L../common/release -lcommon
win32-msvc2008:release:  LIBS += -L../common/release -lcommon
win32-g++:release:LIBS += -L../common/release -lcommon

win32-msvc2005:DEFINES += GLEW_STATIC
win32-msvc2008:DEFINES += GLEW_STATIC 

#DEFINES += GLEW_STATIC

INCLUDEPATH += . .. ../../../vcglib $$GLEWDIR/include
CONFIG += stl 
CONFIG += console

# Mac specific Config required to avoid to make application bundles
CONFIG -= app_bundle
