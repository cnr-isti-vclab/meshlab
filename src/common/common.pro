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
	globals.h \
	interfaces/decorate_plugin_interface.h \
	interfaces/edit_plugin_interface.h \
	interfaces/filter_plugin_interface.h \
	interfaces/iomesh_plugin_interface.h \
	interfaces/mainwindow_interface.h \
	interfaces/plugin_interface.h \
	interfaces/render_plugin_interface.h \
	ml_document/helpers/mesh_document_state_data.h \
	ml_document/helpers/mesh_model_state_data.h \
	ml_document/base_types.h \
	ml_document/cmesh.h \
	ml_document/mesh_model.h \
	ml_document/mesh_model_state.h \
	ml_document/mesh_document.h \
	ml_document/raster_model.h \
	ml_document/render_raster.h \
	parameters/rich_parameter_list.h \
	parameters/value.h \
	parameters/rich_parameter.h \
	utilities/file_format.h \
	GLExtensionsManager.h \
	filterscript.h \
	GLLogStream.h \
	plugin_manager.h \
	mlexception.h \
	mlapplication.h \
	meshlabdocumentxml.h \
	ml_shared_data_context.h \
	ml_selection_buffers.h \
	meshlabdocumentxml.h

SOURCES += \
	globals.cpp \
	interfaces/decorate_plugin_interface.cpp \
	interfaces/filter_plugin_interface.cpp \
	interfaces/plugin_interface.cpp \
	ml_document/helpers/mesh_document_state_data.cpp \
	ml_document/cmesh.cpp \
	ml_document/mesh_model.cpp \
	ml_document/mesh_model_state.cpp \
	ml_document/mesh_document.cpp \
	ml_document/raster_model.cpp \
	ml_document/render_raster.cpp \
	parameters/rich_parameter.cpp \
	parameters/rich_parameter_list.cpp \
	parameters/value.cpp \
	GLExtensionsManager.cpp \
	filterscript.cpp \
	GLLogStream.cpp \
	plugin_manager.cpp \
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
