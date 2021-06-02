include (../general.pri)

QT += gui
QT += opengl
QT += xml
QT += network

#CONFIG += debug_and_release
DESTDIR = $$MESHLAB_DISTRIB_DIRECTORY

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

INCLUDEPATH *= \
	. \
	.. \
	../.. \
	$$VCGDIR \
	$$EIGENDIR

!CONFIG(system_glew): INCLUDEPATH *= $$GLEWDIR/include

DEPENDPATH += \
	$$VCGDIR \
	$$VCGDIR/vcg \
	$$VCGDIR/wrap

HEADERS += \
	mainwindow.h \
	glarea.h \
	dialogs/about_dialog.h \
	dialogs/congrats_dialog.h \
	dialogs/filter_script_dialog.h \
	dialogs/options_dialog.h \
	dialogs/plugin_info_dialog.h \
	dialogs/save_mesh_attributes_dialog.h \
	dialogs/save_snapshot_dialog.h \
	dialogs/setting_dialog.h \
	multiViewer_Container.h \
	glarea_setting.h \
	layerDialog.h \
	ml_std_par_dialog.h \
	additionalgui.h \
	snapshotsetting.h \
	ml_render_gui.h \
	ml_rendering_actions.h \
	ml_default_decorators.h \
	$$VCGDIR/wrap/gui/trackball.h \
	$$VCGDIR/wrap/gui/trackmode.h \
	$$VCGDIR/wrap/gl/trimesh.h \
	rich_parameter_gui/richparameterlistdialog.h \
	rich_parameter_gui/richparameterlistframe.h \
	rich_parameter_gui/richparameterwidgets.h

SOURCES += \
	main.cpp \
	mainwindow_Init.cpp \
	mainwindow_RunTime.cpp \
	glarea.cpp \
	dialogs/about_dialog.cpp \
	dialogs/congrats_dialog.cpp \
	dialogs/filter_script_dialog.cpp \
	dialogs/options_dialog.cpp \
	dialogs/plugin_info_dialog.cpp \
	dialogs/save_mesh_attributes_dialog.cpp \
	dialogs/save_snapshot_dialog.cpp \
	dialogs/setting_dialog.cpp \
	multiViewer_Container.cpp \
	layerDialog.cpp \
	ml_std_par_dialog.cpp \
	additionalgui.cpp \
	ml_render_gui.cpp \
	ml_rendering_actions.cpp \
	ml_default_decorators.cpp \
	$$VCGDIR/wrap/gui/trackball.cpp \
	$$VCGDIR/wrap/gui/trackmode.cpp \
	$$VCGDIR/wrap/gui/coordinateframe.cpp \
	glarea_setting.cpp \
	rich_parameter_gui/richparameterlistdialog.cpp \
	rich_parameter_gui/richparameterlistframe.cpp \
	rich_parameter_gui/richparameterwidgets.cpp

FORMS += \
  dialogs/about_dialog.ui \
  dialogs/congrats_dialog.ui \
	dialogs/filter_script_dialog.ui \
	dialogs/plugin_info_dialog.ui \
	dialogs/save_mesh_attributes_dialog.ui \
	dialogs/save_snapshot_dialog.ui \
	ui/layerDialog.ui

RESOURCES += \
	meshlab.qrc

win32-msvc2005: RCC_DIR = $(ConfigurationName)
win32-msvc2008: RCC_DIR = $(ConfigurationName)

# to add windows icon
win32:RC_FILE = meshlab.rc

# ## the xml info list
# ## the next time the app open a new extension
QMAKE_INFO_PLIST = Info.plist

# to add MacOS icon
ICON = images/meshlab.icns

# note that to add the file icons on the mac the following line does not work.
# You have to copy the file by hand into the meshlab.app/Contents/Resources directory.
# ICON += images/meshlab_obj.icns

CONFIG += stl

LIBS += -L$$MESHLAB_DISTRIB_EXT_DIRECTORY

win32-msvc:LIBS += \
	#-L$$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc \
	-L$$MESHLAB_DISTRIB_DIRECTORY/lib -lmeshlab-common -lopengl32 -lGLU32

win32-g++:LIBS += \
	#-L$$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc \
	-L$$MESHLAB_DISTRIB_DIRECTORY/lib -lmeshlab-common -lopengl32 -lGLU32

macx:LIBS += \
	#-L$$MESHLAB_DISTRIB_DIRECTORY/lib/macx64 \
	$$MESHLAB_DISTRIB_DIRECTORY/lib/libmeshlab-common.dylib

macx:QMAKE_POST_LINK += "\
	cp -P $$MESHLAB_DISTRIB_DIRECTORY/lib/libmeshlab-common.1.dylib $$MESHLAB_DISTRIB_DIRECTORY/meshlab.app/Contents/Frameworks; \
	install_name_tool -change libmeshlab-common.1.dylib @rpath/libmeshlab-common.1.dylib $$MESHLAB_DISTRIB_DIRECTORY/meshlab.app/Contents/MacOS/meshlab \
	"


linux:LIBS += \
	-lmeshlab-common -lGLU #\
	#-L$$MESHLAB_DISTRIB_DIRECTORY/lib/linux

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
