# this is the common include for all the plugins
include (../../shared.pri)
 
 
QT += opengl
QT += xml

HEADERS = 	edit_ocme_factory.h \
    		ui_ocme.h \
    		edit_ocme.h \
		./src/ocme/vcg_mesh.h \
		./src/ocme/type_traits.h \
		./src/ocme/stdmatrix3.h \
		./src/ocme/stdmatrix.h \
		./src/ocme/plane_box_quantization.h \
		./src/ocme/plane_box_intersection.h \
		./src/ocme/ocme_impostor.h \
		./src/ocme/ocme_extract.h \
		./src/ocme/ocme_disk_loader.h \
		./src/ocme/ocme_definition.h \
		./src/ocme/ocme_commit.h \
		./src/ocme/ocme_assert.h \
		./src/ocme/ocme_add.h \
		./src/ocme/ocme.h \
		./src/ocme/multiple_std_iterator.h \
		./src/ocme/impostor_definition.h \
		./src/ocme/impostor_create.h \
		./src/ocme/import_ply_to_ocme.h \
		./src/ocme/import_ocm_ply.h \
		./src/ocme/cell_attributes.h \
		./src/ocme/cell.h \
		./src/ocme/boolvector.h \
		../src/utils/name_access_function_bounds.h

SOURCES = edit_ocme_factory.cpp \
    edit_ocme.cpp \ # ../../../../meshlab/src/meshlab/glarea.cpp \
    ../../../../vcglib/wrap/gui/trackmode.cpp \
    ../../../../vcglib/wrap/gui/trackball.cpp # ../../../../vcglib/wrap/gui/rubberband.cpp \

TARGET = edit_ocme
QT += opengl
RESOURCES = edit_ocme.qrc \
    edit_ocme.qrc
FORMS += ocme.ui

# OCME src/ocme FILES
# Lib headers
INCLUDEPATH += ./src/ocme/
INCLUDEPATH += .
INCLUDEPATH +=    . \
    			../utils/ \
			./src/ \
			./src/ooc_vector/ \

SOURCES += ./src/ocme/ocme.cpp \
		./src/ocme/ocme_render.cpp \
		./src/ocme/ocme_impostor.cpp \
		./src/ocme/ocme_garbage.cpp \
		./src/ocme/ocme_disk_loader.cpp \
		./src/ocme/ocme_debug.cpp \
		./src/ooc_vector/ooc_chains.cpp \
		./src/ooc_vector/berkeleydb/ooc_chunks_berkeleydb.cpp \
		./src/ooc_vector/berkeleydb/berkeleydb.cpp \
		./src/utils/string_serialize.cpp \
		./src/ocme/impostor_create.cpp \
		./src/ocme/impostor_render.cpp \
		./src/ocme/cell.cpp \
		./src/ooc_vector/cache_policy.cpp \
		./src/ooc_vector/simpledb.cpp \
		./src/ocme/berkeleydb/ocme_berkeleyDb.cpp

# Input
HEADERS += ./src/ocme/ocme_definition.h \
		./src/ocme/ocme_commit.h \
		./src/ocme/ocme_add.h \
		./src/ocme/ocme.h \
		./src/ocme/ocme_commit.h \
    		../utils/release_assert.h \
		./src/ocme/FBool.h \
		./src/ocme/berkeleydb/ooc_chunks_berkeleydb.hpp

DEFINES +=  NO_BERKELEY

