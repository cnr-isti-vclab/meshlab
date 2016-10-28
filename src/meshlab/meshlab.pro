include (../general.pri)
#CONFIG += debug_and_release
DESTDIR = ../distrib
EXIF_DIR = ../external/jhead-2.95

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

INCLUDEPATH *= . \
	.. \
	../.. \
    $$VCGDIR \
    $$GLEWDIR/include \
	$$EXIF_DIR
DEPENDPATH += $$VCGDIR \
    $$VCGDIR/vcg \
    $$VCGDIR/wrap
HEADERS = ../common/interfaces.h \
    mainwindow.h \
    glarea.h \
    multiViewer_Container.h \
    glarea_setting.h \
    plugindialog.h \
    customDialog.h \
    filterScriptDialog.h \
    saveSnapshotDialog.h \
    savemaskexporter.h \
    changetexturename.h \
    layerDialog.h \
    stdpardialog.h \
	ml_std_par_dialog.h \
	xmlstdpardialog.h \
	additionalgui.h \
	xmlgeneratorgui.h \
	snapshotsetting.h \
	ml_render_gui.h \
	ml_rendering_actions.h \
	ml_default_decorators.h \
	ml_selection_buffers.h \
    $$VCGDIR/wrap/gui/trackball.h \
    $$VCGDIR/wrap/gui/trackmode.h \
	$$VCGDIR/wrap/gl/trimesh.h \
	filterthread.h 
SOURCES = main.cpp \
    mainwindow_Init.cpp \
    mainwindow_RunTime.cpp \
    glarea.cpp \
	multiViewer_Container.cpp \
    plugindialog.cpp \
    customDialog.cpp \
    filterScriptDialog.cpp \
    saveSnapshotDialog.cpp \
    layerDialog.cpp \
    savemaskexporter.cpp \
    changetexturename.cpp \
    stdpardialog.cpp \
	ml_std_par_dialog.cpp \
    xmlstdpardialog.cpp \
	additionalgui.cpp \
	xmlgeneratorgui.cpp \
	ml_render_gui.cpp \
	ml_rendering_actions.cpp \
	ml_default_decorators.cpp \
	ml_selection_buffers.cpp \
	$$VCGDIR/wrap/gui/trackball.cpp \
    $$VCGDIR/wrap/gui/trackmode.cpp \
	$$VCGDIR/wrap/gui/coordinateframe.cpp \
	#$$GLEWDIR/src/glew.c \
    glarea_setting.cpp \
	filterthread.cpp 

FORMS = ui/layerDialog.ui \
    ui/filterScriptDialog.ui \
    ui/customDialog.ui \
    ui/savesnapshotDialog.ui \
    ui/aboutDialog.ui \
    ui/renametexture.ui \
    ui/savemaskexporter.ui \
    ui/congratsDialog.ui \
	ui/filtergui.ui \
	ui/filtercreatortab.ui
	

	
	win32-msvc2005: RCC_DIR = $(ConfigurationName)
	win32-msvc2008: RCC_DIR = $(ConfigurationName)
	#win32-msvc2010: RCC_DIR = $(ConfigurationName)
	#win32-msvc2012: RCC_DIR = $(ConfigurationName)
	#win32-msvc2013: RCC_DIR = $(ConfigurationName)	


RESOURCES = meshlab.qrc



# to add windows icon
win32:RC_FILE = meshlab.rc

# ## the xml info list
# ## the next time the app open a new extension
QMAKE_INFO_PLIST = ../install/info.plist

# to add MacOS icon
ICON = images/meshlab.icns

# note that to add the file icons on the mac the following line does not work.
# You have to copy the file by hand into the meshlab.app/Contents/Resources directory.
# ICON += images/meshlab_obj.icns
QT += opengl
QT += xml
QT += xmlpatterns
QT += network
QT += script


# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

# the following line is to hide the hundred of warnings about the deprecated
# old printf are all around the code
win32-msvc2005:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32-msvc2008:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32-msvc2010:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32-msvc2012:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32-msvc2013:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32-msvc2015:DEFINES += _CRT_SECURE_NO_DEPRECATE

# Uncomment these if you want to experiment with newer gcc compilers
# (here using the one provided with macports)
# macx-g++:QMAKE_CXX=g++-mp-4.3
# macx-g++:QMAKE_CXXFLAGS_RELEASE -= -Os
# macx-g++:QMAKE_CXXFLAGS_RELEASE += -O3

CONFIG += stl

macx:LIBS		+= -L../external/lib/macx -ljhead ../common/libcommon.dylib
macx32:LIBS		+= -L../external/lib/macx32 -ljhead ../common/libcommon.dylib
macx64:LIBS		+= -L../external/lib/macx64 -ljhead ../common/libcommon.dylib
macx:QMAKE_POST_LINK ="cp -P ../common/libcommon.1.dylib ../distrib/meshlab.app/Contents/MacOS; install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib ../distrib/meshlab.app/Contents/MacOS/meshlab"

win32-msvc2005:LIBS		+= -L../external/lib/win32-msvc2005 -ljhead -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2008:LIBS		+= -L../external/lib/win32-msvc2008 -ljhead -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2010:LIBS		+= -L../external/lib/win32-msvc2010 -ljhead -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2012:LIBS		+= -L../external/lib/win32-msvc2012 -ljhead -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2013:LIBS		+= -L../external/lib/win32-msvc2013 -ljhead -L../distrib -lcommon -lopengl32 -lGLU32
win32-msvc2015:LIBS		+= -L../external/lib/win32-msvc2015 -ljhead -L../distrib -lcommon -lopengl32 -lGLU32
win32-g++:LIBS        	+= -L../external/lib/win32-gcc -ljhead -L../distrib -lcommon -lopengl32 -lGLU32

#CONFIG(release,debug | release) {
#	win32-msvc2005:release:LIBS     += -L../common/release -lcommon
#	win32-msvc2008:release:LIBS     += -L../common/release -lcommon
#	win32-g++:release:LIBS 			+= -L../common/release -lcommon
#}

linux-g++:LIBS += -L../external/lib/linux-g++ -ljhead -L../distrib -lcommon -lGLU
linux-g++:QMAKE_RPATHDIR += ../distrib
linux-g++-32:LIBS += -L../external/lib/linux-g++-32 -ljhead -L../distrib -lcommon -lGLU
linux-g++-32:QMAKE_RPATHDIR += ../distrib
linux-g++-64:LIBS += -L../external/lib/linux-g++-64 -ljhead -L../distrib -lcommon -lGLU
linux-g++-64:QMAKE_RPATHDIR += ../distrib

# uncomment in your local copy only in emergency cases.
# We should never be too permissive
# win32-g++:QMAKE_CXXFLAGS += -fpermissive

# The following define is needed in gcc to remove the asserts
win32-g++:DEFINES += NDEBUG
CONFIG(debug, debug|release):win32-g++:release:DEFINES -= NDEBUG
