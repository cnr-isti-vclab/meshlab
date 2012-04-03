include (../../shared.pri)
 
 
QT += opengl
QT += xml
TARGET = edit_ocme
CONFIG += debug_and_release

#CONFIG += SIMPLE_DB


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
                ../src/utils/name_access_function_bounds.h\
                ./src/ocme/ocme_definition.h \
                ./src/ocme/ocme_commit.h \
                ./src/ocme/ocme_add.h \
                ./src/ocme/ocme.h \
                ./src/ocme/ocme_commit.h \
                ../utils/release_assert.h \
                ./src/ocme/FBool.h \



 
RESOURCES = edit_ocme.qrc \
		    edit_ocme.qrc

FORMS += ocme.ui
DEFINES += __STDC_LIMIT_MACROS
INCLUDEPATH += ./src/ocme/
INCLUDEPATH += .
INCLUDEPATH +=      . \
                    ../utils/ \
                    ./src/ \
                    ./src/ooc_vector \
                    ../../external/kyotocabinet-1.2.34/include

#SIMPLE_DB{
#DEFINES += SIMPLE_DB
#HEADERS += ./src/ocme/io/ooc_chunks.hpp
#
#SOURCES +=            	./src/ooc_vector/io/ooc_chunks.cpp
#SOURCES +=              ./src/ooc_vector/simpledb.cpp
#SOURCES +=              ./src/ocme/simpledb/ocme_simpledb.cpp
#}
#else
#{
HEADERS += ./src/ocme/io/ooc_chunks_kcdb.hpp
SOURCES +=            	./src/ooc_vector/io/ooc_chunks_kcdb.cpp \
                        ./src/ocme/simpledb/ocme_kcdb.cpp
#}

SOURCES +=              ./src/ooc_vector/ooc_chains.cpp\
                        ./src/ocme/ocme.cpp \
			./src/ocme/ocme_render.cpp \
			./src/ocme/ocme_impostor.cpp \
			./src/ocme/ocme_garbage.cpp \
			./src/ocme/ocme_disk_loader.cpp \
			./src/ocme/ocme_debug.cpp \
			./src/utils/string_serialize.cpp \
			./src/ocme/impostor_create.cpp \
			./src/ocme/impostor_render.cpp \
			./src/ocme/cell.cpp \
                        ./src/ooc_vector/cache_policy.cpp


SOURCES +=               edit_ocme_factory.cpp \
                        edit_ocme.cpp \
                        ../../../../vcglib/wrap/gui/trackmode.cpp \
                        ../../../../vcglib/wrap/gui/trackball.cpp

LIBS += -lkyotocabinet
LIBPATH +=  /../../external/kyotocabinet-1.2.34

