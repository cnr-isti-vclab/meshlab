VCGDIR = ../../../vcglib
GLEWDIR = ../external/glew-1.5.1
#CONFIG += debug_and_release
DESTDIR = ../distrib
# uncomment to try Eigen
# DEFINES += VCG_USE_EIGEN
# CONFIG += warn_off
INCLUDEPATH *= ../.. \
    $$VCGDIR \
    $$GLEWDIR/include
DEPENDPATH += $$VCGDIR \
    $$VCGDIR/vcg \
    $$VCGDIR/wrap
HEADERS = ../common/interfaces.h \
    mainwindow.h \
    glarea.h \
    glarea_setting.h \
    plugindialog.h \
    customDialog.h \
    filterScriptDialog.h \
    saveSnapshotDialog.h \
    savemaskexporter.h \
    changetexturename.h \
    layerDialog.h \
    stdpardialog.h \
    $$VCGDIR/wrap/gui/trackball.h \
    $$VCGDIR/wrap/gui/trackmode.h \
    $$VCGDIR/wrap/gl/trimesh.h
SOURCES = main.cpp \
    mainwindow_Init.cpp \
    mainwindow_RunTime.cpp \
    glarea.cpp \
    plugindialog.cpp \
    customDialog.cpp \
    filterScriptDialog.cpp \
    saveSnapshotDialog.cpp \
    layerDialog.cpp \
    savemaskexporter.cpp \
    changetexturename.cpp \
    stdpardialog.cpp \
    $$VCGDIR/wrap/gui/trackball.cpp \
    $$VCGDIR/wrap/gui/trackmode.cpp \
    glarea_setting.cpp

FORMS = ui/layerDialog.ui \
    ui/filterScriptDialog.ui \
    ui/customDialog.ui \
    ui/savesnapshotDialog.ui \
    ui/aboutDialog.ui \
    ui/renametexture.ui \
    ui/savemaskexporter.ui \
    ui/congratsDialog.ui
RESOURCES = meshlab.qrc

# to add windows icon
RC_FILE = meshlab.rc

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
QT += network

# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

# the following line is to hide the hundred of warnings about the deprecated
# old printf are all around the code
win32-msvc2005:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32-msvc2008:DEFINES += _CRT_SECURE_NO_DEPRECATE
mac:QMAKE_CXX = g++-4.2

# Uncomment these if you want to experiment with newer gcc compilers
# (here using the one provided with macports)
# macx-g++:QMAKE_CXX=g++-mp-4.3
# macx-g++:QMAKE_CXXFLAGS_RELEASE -= -Os
# macx-g++:QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += . \
    .. \
    ../../../vcglib \
    $$GLEWDIR/include
CONFIG += stl

macx:LIBS += ../common/libcommon.dylib
macx:QMAKE_POST_LINK ="cp -P ../common/libcommon.1.dylib ../distrib/meshlab.app/Contents/MacOS; install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib ../distrib/meshlab.app/Contents/MacOS/meshlab"




win32-msvc2005:LIBS		+= -L../distrib -lcommon
win32-msvc2008:LIBS		+= -L../distrib -lcommon
win32-g++:LIBS        	+= -L../distrib -lcommon

#CONFIG(release,debug | release) {
#	win32-msvc2005:release:LIBS     += -L../common/release -lcommon
#	win32-msvc2008:release:LIBS     += -L../common/release -lcommon
#	win32-g++:release:LIBS 			+= -L../common/release -lcommon
#}

linux-g++:LIBS += -L../distrib -lcommon
linux-g++:QMAKE_RPATHDIR += ../distrib
linux-g++-32:LIBS += -L../distrib -lcommon
linux-g++-32:QMAKE_RPATHDIR += ../distrib
linux-g++-64:LIBS += -L../distrib -lcommon
linux-g++-64:QMAKE_RPATHDIR += ../distrib


# uncomment in your local copy only in emergency cases.
# We should never be too permissive
# win32-g++:QMAKE_CXXFLAGS += -fpermissive

# The following define is needed in gcc to remove the asserts
win32-g++:DEFINES += NDEBUG
CONFIG(debug, debug|release):win32-g++:release:DEFINES -= NDEBUG
