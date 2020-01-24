include (../general.pri)

QT += opengl
QT += xml
QT += xmlpatterns
QT += network
QT += script

#CONFIG += debug_and_release
DESTDIR = $$MESHLAB_DISTRIB_DIRECTORY
EXIF_DIR = ../external/jhead-3.04

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

INCLUDEPATH *= \
    . \
	.. \
	../.. \
    $$VCGDIR \
    $$EIGENDIR \
	$$EXIF_DIR

!CONFIG(system_glew): INCLUDEPATH *= $$GLEWDIR/include

DEPENDPATH += \
    $$VCGDIR \
    $$VCGDIR/vcg \
    $$VCGDIR/wrap

HEADERS = \
    ../common/interfaces.h \
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
    $$VCGDIR/wrap/gui/trackball.h \
    $$VCGDIR/wrap/gui/trackmode.h \
	$$VCGDIR/wrap/gl/trimesh.h \
	filterthread.h 

SOURCES = \
    main.cpp \
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
	$$VCGDIR/wrap/gui/trackball.cpp \
    $$VCGDIR/wrap/gui/trackmode.cpp \
	$$VCGDIR/wrap/gui/coordinateframe.cpp \
    glarea_setting.cpp \
	filterthread.cpp 

FORMS += \
    ui/layerDialog.ui \
    ui/filterScriptDialog.ui \
    ui/customDialog.ui \
    ui/savesnapshotDialog.ui \
    ui/aboutDialog.ui \
    ui/renametexture.ui \
    ui/savemaskexporter.ui \
    ui/congratsDialog.ui \
	ui/filtergui.ui \
	ui/filtercreatortab.ui
	
RESOURCES = meshlab.qrc

win32-msvc2005: RCC_DIR = $(ConfigurationName)
win32-msvc2008: RCC_DIR = $(ConfigurationName)

# to add windows icon
win32:RC_FILE = meshlab.rc

# ## the xml info list
# ## the next time the app open a new extension
QMAKE_INFO_PLIST = $$MESHLAB_SOURCE_DIRECTORY/../install/macx/Info.plist

# to add MacOS icon
ICON = images/meshlab.icns

# note that to add the file icons on the mac the following line does not work.
# You have to copy the file by hand into the meshlab.app/Contents/Resources directory.
# ICON += images/meshlab_obj.icns


# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

CONFIG += stl

win32-msvc:LIBS += \
    -L$${MESHLAB_DISTRIB_DIRECTORY}/lib/win32-msvc -ljhead \
    -L$${MESHLAB_DISTRIB_DIRECTORY}/lib -lcommon -lopengl32 -lGLU32

win32-g++:LIBS += \
    -L$${MESHLAB_DISTRIB_DIRECTORY}/lib/win32-gcc -ljhead \
    -L$${MESHLAB_DISTRIB_DIRECTORY}/lib -lcommon -lopengl32 -lGLU32

macx:LIBS += \
    -L$${MESHLAB_DISTRIB_DIRECTORY}/lib/macx64 -ljhead \
    $${MESHLAB_DISTRIB_DIRECTORY}/lib/libcommon.dylib

macx:QMAKE_POST_LINK = "\
    cp -P $${MESHLAB_DISTRIB_DIRECTORY}/lib/libcommon.1.dylib $${MESHLAB_DISTRIB_DIRECTORY}/meshlab.app/Contents/MacOS; \
    install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib $${MESHLAB_DISTRIB_DIRECTORY}/meshlab.app/Contents/MacOS/meshlab \
    "

#CONFIG(release,debug | release) {
#	win32-msvc2005:release:LIBS     += -L../common/release -lcommon
#	win32-msvc2008:release:LIBS     += -L../common/release -lcommon
#	win32-g++:release:LIBS 			+= -L../common/release -lcommon
#}

linux:LIBS += \
    -lcommon -lGLU \
    -L$${MESHLAB_DISTRIB_DIRECTORY}/lib/linux-g++ -ljhead

!CONFIG(system_glew) {
	INCLUDEPATH *= $$GLEWDIR/include
}
CONFIG(system_glew): LIBS += -lGLEW

# uncomment in your local copy only in emergency cases.
# We should never be too permissive
# win32-g++:QMAKE_CXXFLAGS += -fpermissive

# The following define is needed in gcc to remove the asserts
win32-g++:DEFINES += NDEBUG
CONFIG(debug, debug|release):win32-g++:release:DEFINES -= NDEBUG
