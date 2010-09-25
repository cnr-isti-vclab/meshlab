# this is the common include for all the plugins
VCGDIR  = ../../../../../../vcglib
GLEWDIR = ../../../../external/glew-1.5.1


win32-msvc2005:DEFINES += GLEW_STATIC
win32-msvc2008:DEFINES += GLEW_STATIC 

INCLUDEPATH  *= ../.. $$VCGDIR $$GLEWDIR/include
DEPENDPATH += $$VCGDIR

# the following line is to hide the hundred of warnings about the deprecated
# old printf are all around the code
win32-msvc2005:DEFINES	+= _CRT_SECURE_NO_DEPRECATE
win32-msvc2008:DEFINES	+= _CRT_SECURE_NO_DEPRECATE


# Base options
TEMPLATE = app
LANGUAGE = C++
CONFIG += console
CONFIG += debug

QT += xml
QT += opengl

# Executable name
TARGET = ocme_builder

# Directories
DESTDIR = .
UI_DIR = build/ui
MOC_DIR = build/moc
OBJECTS_DIR = build/obj

# Lib headers
# Lib
# Awful problem with windows..
win32 { 
    DEFINES += NOMINMAX
    DEFINES += _CRT_SECURE_NO_WARNINGS
}

DEFINES += NO_BERKELEY
DEFINES += _DEBUG

# Input
HEADERS += ocme.h \
    ocme_definition.h \
    ocme_assert.h \
    ocme_add.h \
    ocme_extract.h \
    cell_attributes.h \
		import_ocm_ply.h \
		../ooc_vector/ooc_chains.h \
		../../../../../../vcglib/wrap/system/getopt.h
HEADERS += cell.h
HEADERS += type_traits.h
HEADERS += ../utils/logging.h
HEADERS += vcg_mesh.h
HEADERS += cell.h
HEADERS += impostor_definition.h \
    ocme_impostor.h \
    ocme_commit.h \
    impostor_create.h
SOURCES += main.cpp \
    ../ooc_vector/cache_policy.cpp \
    ../ooc_vector/ooc_chains.cpp \
    ../ooc_vector/berkeleydb/ooc_chunks_berkeleydb.cpp \
    ../ooc_vector/berkeleydb/berkeleyDb.cpp \
    ../utils/string_serialize.cpp \
    ../ooc_vector/simpledb.cpp \
		../../../../../../vcglib/wrap/system/getopt.cpp
SOURCES += cell.cpp
SOURCES += ocme.cpp
SOURCES += ocme_garbage.cpp
SOURCES += ocme_debug.cpp
SOURCES += berkeleydb/ocme_berkeleyDb.cpp
SOURCES += ../../../../../../vcglib/wrap/ply/plylib.cpp \
    impostor_create.cpp \
    ocme_disk_loader.cpp \
    ocme_impostor.cpp \
    plane_box_quantization.cpp
SOURCES -= core.pro
HEADERS -= chain_mem_berkeleydSb.h
INCLUDEPATH += ./berkeleydb \
    . \
    ../utils/ \
    ../ooc_vector/ \
    ../ooc_vector/berkeleydb \
    /usr/local/BerkeleyDB.4.7/include \
		../../../../../../vcglib/ \
    /usr/include/qt4/Qt \
		../  
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG
#LIBS += -L/usr/lib \
#    -L/usr/local/BerkeleyDB.4.7/lib \
LIBS +=    -lGLEW 

LIBPATH += $$GLEWDIR/lib

#    -ldb_cxx-4.7
 
