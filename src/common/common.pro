include (../general.pri)

QT += opengl
QT += xml
QT += network

TEMPLATE = lib
TARGET = meshlab-common
DEPENDPATH += .

DESTDIR = $$MESHLAB_DISTRIB_DIRECTORY/lib

win32-msvc:CONFIG += staticlib
win32-g++:DLLDESTDIR = $$MESHLAB_DISTRIB_DIRECTORY/lib

linux:CONFIG += dll

INCLUDEPATH *= \
	../.. \
	$$$$MESHLAB_EXTERNAL_DIRECTORY/easyexif \
	$$VCGDIR \
	$$EIGENDIR

!CONFIG(system_glew) {
	INCLUDEPATH *= $$GLEWDIR/include
	GLEWCODE = $$GLEWDIR/src/glew.c
}

!CONFIG(system_glew) {
	SOURCES += $$GLEWCODE
	DEFINES += GLEW_STATIC
}

# defining meshlab version
exists(../../ML_VERSION){
	MESHLAB_VERSION = $$cat(../../ML_VERSION)
	message(MeshLab Version: $$MESHLAB_VERSION)
	DEFINES += "MESHLAB_VERSION=$$MESHLAB_VERSION"
}

# Input
HEADERS += 	\
	GLExtensionsManager.h \
	filter_parameter/rich_parameter_list.h \
	filter_parameter/value.h \
	filter_parameter/rich_parameter.h \
	filterscript.h \
	GLLogStream.h \
	interfaces/decorate_plugin_interface.h \
	interfaces/edit_plugin_interface.h \
	interfaces/filter_plugin_interface.h \
	interfaces/io_plugin_interface.h \
	interfaces/mainwindow_interface.h \
	interfaces/plugin_interface.h \
	interfaces/render_plugin_interface.h \
	mesh_data_structures/cmesh.h \
	mesh_data_structures/mesh_model.h \
	mesh_data_structures/mesh_document.h \
	mesh_data_structures/raster_model.h \
	mesh_data_structures/render_raster.h \
	pluginmanager.h \
	mlexception.h \
	mlapplication.h \
	meshlabdocumentxml.h \
	ml_shared_data_context.h \
	ml_selection_buffers.h \
	meshlabdocumentxml.h

SOURCES += \
	GLExtensionsManager.cpp \
	filter_parameter/rich_parameter.cpp \
	filter_parameter/rich_parameter_list.cpp \
	filter_parameter/value.cpp \
	filterscript.cpp \
	GLLogStream.cpp \
	interfaces/decorate_plugin_interface.cpp \
	interfaces/filter_plugin_interface.cpp \
	interfaces/plugin_interface.cpp \
	mesh_data_structures/cmesh.cpp \
	mesh_data_structures/mesh_model.cpp \
	mesh_data_structures/mesh_document.cpp \
	mesh_data_structures/raster_model.cpp \
	mesh_data_structures/render_raster.cpp \
	pluginmanager.cpp \
	mlapplication.cpp \
	searcher.cpp \
	meshlabdocumentxml.cpp \
	meshlabdocumentbundler.cpp \
	ml_shared_data_context.cpp \
	ml_selection_buffers.cpp \
	$$MESHLAB_EXTERNAL_DIRECTORY/easyexif/exif.cpp

macx:QMAKE_POST_LINK = "\
	if [ -d  $$MESHLAB_DISTRIB_DIRECTORY/meshlab.app/Contents/Frameworks/ ]; \
	then  \
		echo "Copying";  \
	else  \
		mkdir -p $$MESHLAB_DISTRIB_DIRECTORY/meshlab.app/Contents/Frameworks;  \
	fi;   \
	cp $$MESHLAB_DISTRIB_DIRECTORY/lib/libmeshlab-common.* $$MESHLAB_DISTRIB_DIRECTORY/meshlab.app/Contents/Frameworks/ ;\
#	if [ -d ../external/ ];\
#	then \
#		echo "ok external dir exists"; \
#	else \
#		ln -s ../../meshlab/src/external ../external;\
#		echo "linked external dir"; \
#	fi;\
#	if [ -d $$MESHLAB_DISTRIB_DIRECTORY/shaders/ ];\
#	then \
#		echo "ok shader dir exists"; \
#	else \
#		ln -s ../../../meshlab/src/distrib/shaders ../distrib/shaders ;\
#		echo "linked shader dir"; \
#	fi;\
	"
