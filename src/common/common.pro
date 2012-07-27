include (../general.pri)
EXIF_DIR = ../external/jhead-2.95


GLEWCODE = $$GLEWDIR/src/glew.c

win32-msvc2005:DESTDIR = ../distrib
win32-msvc2008:DESTDIR = ../distrib
win32-msvc2010:DESTDIR = ../distrib
win32-g++:DLLDESTDIR = ../distrib


# CONFIG(release,debug | release) {
# win32-msvc2005:DESTDIR     = ./release
# win32-msvc2008:DESTDIR     = ./release
# win32-msvc2010:DESTDIR     = ./release
# win32-g++:DLLDESTDIR     = ./release
# }

mac:DLLDESTDIR = .

# The following lines are necessary to avoid that when you re-compile everything you still find old dll in the plugins dir
macx:QMAKE_CLEAN +=  ../distrib/plugins/*.dylib
win32:QMAKE_CLEAN +=  ../distrib/plugins/*.dll
linux-g++:QMAKE_CLEAN +=  ../distrib/plugins/*.so


macx:QMAKE_POST_LINK = "\
if [ -d  ../distrib/meshlab.app/Contents/MacOS/ ]; \
then  \
echo "Copying";  \
else  \
mkdir ../distrib;  \
mkdir ../distrib/meshlab.app ;  \
mkdir ../distrib/meshlab.app/Contents;  \
mkdir ../distrib/meshlab.app/Contents/MacOS;  \
fi;   \
cp libcommon.* ../distrib/meshlab.app/Contents/MacOS/ ;\
if [ -d ../external/ ];\
then \
echo "ok shader dir exists"; \
else \
ln -s ../../meshlab/src/external ../external;\
echo "linked external dir"; \
fi;\
if [ -d ../distrib/shaders/ ];\
then \
echo "ok shader dir exists"; \
else \
ln -s ../../../meshlab/src/distrib/shaders ../distrib/shaders ;\
echo "linked shader dir"; \
fi;\
"
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
win32-msvc2010:CONFIG += staticlib

QT += opengl
QT += xml
QT += xmlpatterns
QT += script


TARGET = 
DEPENDPATH += .
INCLUDEPATH += . $$EXIF_DIR
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
	xmlfilterinfo.h \
	mlexception.h \
	mlapplication.h \
	scriptsyntax.h \
	searcher.h \
	$$VCGDIR/wrap/gl/trimesh.h \
    meshlabdocumentxml.h 
SOURCES += filterparameter.cpp \
    interfaces.cpp \
    filterscript.cpp \
    GLLogStream.cpp \
    meshmodel.cpp \
    pluginmanager.cpp \
	scriptinterface.cpp \
	xmlfilterinfo.cpp \
	mlapplication.cpp \
	scriptsyntax.cpp \
	searcher.cpp \
    $$GLEWCODE \
    meshlabdocumentxml.cpp \
    meshlabdocumentbundler.cpp

#	win32-msvc2005: RCC_DIR = $(ConfigurationName)
#	win32-msvc2008: RCC_DIR = $(ConfigurationName)
#	win32-msvc2010: RCC_DIR = $(ConfigurationName)

macx:LIBS		+= -L../external/lib/macx -ljhead
macx32:LIBS		+= -L../external/lib/macx32 -ljhead
macx64:LIBS		+= -L../external/lib/macx64 -ljhead
win32-msvc.net:LIBS	+= ../external/lib/win32-msvc.net/jhead.lib
win32-msvc2005:LIBS	+= ../external/lib/win32-msvc2005/jhead.lib
win32-msvc2008:LIBS	+= ../external/lib/win32-msvc2008/jhead.lib
win32-g++:LIBS		+= -L../external/lib/win32-gcc -ljhead
linux-g++:LIBS		+= -L../external/lib/linux-g++ -ljhead
linux-g++-32:LIBS		+= -L../external/lib/linux-g++-32 -ljhead
linux-g++-64:LIBS		+= -L../external/lib/linux-g++-64 -ljhead
	
#RESOURCES = common.qrc
