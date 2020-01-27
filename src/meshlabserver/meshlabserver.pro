include (../general.pri)

INCLUDEPATH += . .. $$VCGDIR $$EIGENDIR

!CONFIG(system_glew): INCLUDEPATH *= $$GLEWDIR/include

SOURCES = mainserver.cpp

RESOURCES = \
    ../meshlab/meshlab.qrc \
    meshlabserver.qrc

# to add windows icon 
RC_FILE = ../meshlab/meshlab.rc

QT += xml opengl
QT += xmlpatterns
QT += script

DESTDIR = $$MESHLAB_DISTRIB_DIRECTORY
macx:DESTDIR = $$MESHLAB_DISTRIB_DIRECTORY/meshlab.app/Contents/MacOS/
macx:QMAKE_POST_LINK ="install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib $$MESHLAB_DISTRIB_DIRECTORY/meshlab.app/Contents/MacOS/meshlabserver"

mac:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/meshlab.app/Contents/MacOS/libcommon.dylib

win32-msvc: LIBS += -lopengl32 -lGLU32 -L$$MESHLAB_DISTRIB_DIRECTORY/lib -lcommon
win32-g++: LIBS += -lcommon
linux:  LIBS += -lcommon

win32-msvc:DEFINES += GLEW_STATIC

CONFIG(system_glew):LIBS += -lGLEW

#DEFINES += GLEW_STATIC

# This removes the need of XSERVER to run meshlabserver
DEFINES += NO_XSERVER_DEPENDENCY

CONFIG += stl 
CONFIG += console

# Mac specific Config required to avoid to make application bundles
CONFIG -= app_bundle
