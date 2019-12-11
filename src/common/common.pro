include (../general.pri)
EXIF_DIR = ../external/jhead-3.04

win32-msvc:DESTDIR = ../distrib
win32-g++:DLLDESTDIR = ../distrib

# The following lines are necessary to avoid that when you re-compile everything you still find old dll in the plugins dir
macx:QMAKE_CLEAN +=  ../distrib/plugins/*.dylib
win32:QMAKE_CLEAN +=  ../distrib/plugins/*.dll
linux-g++:QMAKE_CLEAN +=  ../distrib/plugins/*.so


macx:QMAKE_POST_LINK = "\
if [ -d  ../distrib/meshlab.app/Contents/MacOS/ ]; \
then  \
echo "Copying";  \
else  \
mkdir -p ../distrib/meshlab.app/Contents/MacOS;  \
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
    $$EXIF_DIR
!CONFIG(system_glew) {
	INCLUDEPATH *= $$GLEWDIR/include
	GLEWCODE = $$GLEWDIR/src/glew.c
}
TEMPLATE = lib

linux:CONFIG += dll
linux:DESTDIR = ../distrib

linux-g++:QMAKE_CXXFLAGS+=-Wno-unknown-pragmas

win32-msvc:CONFIG += staticlib

QT += opengl
QT += xml
QT += xmlpatterns
QT += script


TARGET = common
DEPENDPATH += .

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
			meshlabdocumentxml.cpp \
			meshlabdocumentbundler.cpp \
			ml_shared_data_context.cpp 

!CONFIG(system_glew) {
	SOURCES += $$GLEWCODE
	DEFINES += GLEW_STATIC
}
