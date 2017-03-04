include (../general.pri)
EXIF_DIR = ../external/jhead-2.95

GLEWCODE = $$GLEWDIR/src/glew.c

win32-msvc2005:DESTDIR = ../distrib
win32-msvc2008:DESTDIR = ../distrib
win32-msvc2010:DESTDIR = ../distrib
win32-msvc2012:DESTDIR = ../distrib
win32-msvc2013:DESTDIR = ../distrib
win32-msvc2015:DESTDIR = ../distrib
win32-g++:DLLDESTDIR = ../distrib

# CONFIG(release,debug | release) {
# win32-msvc2005:DESTDIR     = ./release
# win32-msvc2008:DESTDIR     = ./release
# win32-msvc2010:DESTDIR     = ./release
# win32-msvc2012:DESTDIR     = ./release
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
    $$EIGENDIR \
    $$GLEWDIR/include
TEMPLATE = lib
linux-g++:CONFIG += dll
linux-g++:DESTDIR = ../distrib
linux-g++-32:CONFIG += dll
linux-g++-32:DESTDIR = ../distrib
linux-g++-64:CONFIG += dll
linux-g++-64:DESTDIR = ../distrib

linux-g++:QMAKE_CXXFLAGS+=-Wno-unknown-pragmas

win32-msvc2005:CONFIG += staticlib
win32-msvc2008:CONFIG += staticlib
win32-msvc2010:CONFIG += staticlib
win32-msvc2012:CONFIG += staticlib
win32-msvc2013:CONFIG += staticlib
win32-msvc2015:CONFIG += staticlib

QT += opengl
QT += xml
QT += xmlpatterns
QT += script


TARGET = common
DEPENDPATH += .
INCLUDEPATH += . $$EXIF_DIR
DEFINES += GLEW_STATIC
win32-msvc2005:DEFINES += _CRT_SECURE_NO_WARNINGS
win32-msvc2008:DEFINES += _CRT_SECURE_NO_WARNINGS
win32-msvc2010:DEFINES += _CRT_SECURE_NO_WARNINGS
win32-msvc2012:DEFINES += _CRT_SECURE_NO_WARNINGS
win32-msvc2013:DEFINES += _CRT_SECURE_NO_WARNINGS
win32-msvc2015:DEFINES += _CRT_SECURE_NO_WARNINGS


# Input
HEADERS += 	filterparameter.h \
			filterscript.h \
			GLLogStream.h \
			interfaces.h \
			ml_mesh_type.h \
			meshmodel.h \
			pluginmanager.h \
			scriptinterface.h \
			xmlfilterinfo.h \
			mlexception.h \
			mlapplication.h \
			scriptsyntax.h \
			meshlabdocumentxml.h \
			ml_shared_data_context.h \
			meshlabdocumentxml.h
			
SOURCES += 	filterparameter.cpp \
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
			meshlabdocumentbundler.cpp \
			ml_shared_data_context.cpp 
