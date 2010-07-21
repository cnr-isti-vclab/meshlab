# this is the common include for all the plugins
TEMPLATE = lib
CONFIG += plugin
QT += opengl
QT += xml
VCGDIR = ../../../../vcglib
GLEWDIR = ../../../../meshlab/src/external/glew-1.5.1
mac:LIBS += ../../../../meshlab/src/common/libcommon.dylib

# correct qmake syntax requires CONFIG(option, list of options)
win32-msvc2005:LIBS += ../../../../meshlab/src/distrib/common.lib
win32-msvc2008:LIBS += ../../../../meshlab/src/distrib/common.lib
win32-g++:LIBS += -L../../../../meshlab/src/distrib \
    -lcommon

# CONFIG(debug, debug|release) {
# win32-msvc2005:  LIBS += ../../common/debug/common.lib
# win32-msvc2008:  LIBS += ../../common/debug/common.lib
# win32-g++:  LIBS += -L../../common/debug -lcommon
# }
win32-msvc2005:DEFINES += GLEW_STATIC
win32-msvc2008:DEFINES += GLEW_STATIC

# uncomment to try Eigen
# DEFINES += VCG_USE_EIGEN
# CONFIG += warn_off
INCLUDEPATH *= ../../../../ \
    $$VCGDIR \
    $$GLEWDIR/include \
    ../../../../meshlab/src \
    ../ \
win32:INCLUDEPATH += ../../../../code/lib/Berkeleydb_4.8.26/include
DEPENDPATH += $$VCGDIR

# Uncomment these if you want to experiment with newer gcc compilers
# (here using the one provided with macports)
# macx-g++:QMAKE_CXX=g++-mp-4.3
# macx-g++:QMAKE_CXXFLAGS_RELEASE -= -Os
# macx-g++:QMAKE_CXXFLAGS_RELEASE += -O3
macx:QMAKE_CXX = g++-4.2

# #macx:QMAKE_POST_LINK ="install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib ../../meshlab/plugins/lib$${TARGET}.dylib"
# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

# the following line is to hide the hundred of warnings about the deprecated
# old printf are all around the code
win32-msvc2005:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32-msvc2008:DEFINES += _CRT_SECURE_NO_DEPRECATE
CONFIG(release,debug | release):

# Uncomment the following line to disable assert in mingw
# DEFINES += NDEBUG
DESTDIR = ../../../../meshlab/src/distrib/plugins

# uncomment in you local copy only in emergency cases.
# We should never be too permissive
# win32-g++:QMAKE_CXXFLAGS += -fpermissive
contains(TEMPLATE,lib):CONFIG(debug, debug|release) { 
		unix:TARGET = $$member(TARGET, 0)_DEBUG
    else:TARGET = $$member(TARGET, 0)d
}
HEADERS = edit_ocme_factory.h \
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

# ../../../../vcglib/wrap/gui/coordinateframe.cpp \
# ../../../../vcglib/wrap/gui/activecoordinateframe.cpp
TARGET = edit_ocme
QT += opengl
RESOURCES = edit_ocme.qrc \
    edit_ocme.qrc
FORMS += ocme.ui

# OCME src/ocme FILES
# Lib headers
INCLUDEPATH += ./src/ocme/
INCLUDEPATH += .
INCLUDEPATH += ./berkeleydb \
    . \
    ../utils/ \
		./src/ \
		./src/ooc_vector/ \
		./src/ooc_vector/berkeleydb \
    /usr/local/BerkeleyDB.4.7/include \
    ../../../../vcglib/ \
    /usr/include/qt4/Qt
win32:INCLUDEPATH += ../../../../code/lib

# Lib sources
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

#LIBS += -L/usr/local/BerkeleyDB.4.7/lib \
#	 -ldb \
#		-ldb_cxx-4.7

# #DEFINES += GLEW_STATIC
DEFINES += _RELEASE_ASSERT_
DEFINES +=  NO_BERKELEY
#DEFINES += _DEBUG
