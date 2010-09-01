VCGDIR = ../../../vcglib
GLEWDIR = ../external/glew-1.5.1
GLEWCODE = $$GLEWDIR/src/glew.c


win32-msvc2005:DESTDIR = ../distrib
win32-msvc2008:DESTDIR = ../distrib
win32-g++:DLLDESTDIR = ../distrib


# CONFIG(release,debug | release) {
# win32-msvc2005:DESTDIR     = ./release
# win32-msvc2008:DESTDIR     = ./release
# win32-g++:DLLDESTDIR     = ./release
# }

mac:DLLDESTDIR = .

# The following lines are necessary to avoid that when you re-compile everything you still find old dll in the plugins dir
macx:QMAKE_CLEAN +=  ../distrib/plugins/*.dylib
win32:QMAKE_CLEAN +=  ../distrib/plugins/*.dll
linux-g++:QMAKE_CLEAN +=  ../distrib/plugins/*.so


macx:QMAKE_POST_LINK = "\
if [ -d  ../distrib/meshlab.app/Contents/MacOs/ ]; \
then  \
echo "Copying";  \
else  \
mkdir ../distrib;  \
mkdir ../distrib/meshlab.app ;  \
mkdir ../distrib/meshlab.app/Contents;  \
mkdir ../distrib/meshlab.app/Contents/MacOs;  \
fi;   \
cp libcommon.* ../distrib/meshlab.app/Contents/MacOs/"
INCLUDEPATH *= ../.. \
    $$VCGDIR \
    $$GLEWDIR/include
TEMPLATE = lib
linux-g++:CONFIG += dll
linux-g++:DESTDIR = ../distrib
linux-g++-32:CONFIG += dll
linux-g++-32:DESTDIR = ../distrib
linux-g++-64:CONFIG += dll
linux-g++-64:DESTDIR = ../distrib

win32-msvc2005:CONFIG += staticlib
win32-msvc2008:CONFIG += staticlib

QT += opengl
QT += xml
QT += script


TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
DEFINES += GLEW_STATIC
win32-msvc2005:DEFINES += _CRT_SECURE_NO_WARNINGS
win32-msvc2008:DEFINES += _CRT_SECURE_NO_WARNINGS


# Input
HEADERS += filterparameter.h \
    filterscript.h \
    GLLogStream.h \
    interfaces.h \
    meshmodel.h \
    pluginmanager.h \
	scriptinterface.h \
    ../../../vcglib/wrap/gl/trimesh.h
SOURCES += filterparameter.cpp \
    interfaces.cpp \
    filterscript.cpp \
    GLLogStream.cpp \
    meshmodel.cpp \
    pluginmanager.cpp \
	scriptinterface.cpp \
    $$GLEWCODE
