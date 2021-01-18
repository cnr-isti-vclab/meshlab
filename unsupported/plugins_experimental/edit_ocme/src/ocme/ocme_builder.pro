# this is the common include for all the plugins
VCGDIR  = ../../../../../../vcglib
GLEWDIR = ../../../../external/glew-1.5.1


win32-msvc2005:DEFINES += GLEW_STATIC
win32-msvc2008:DEFINES += GLEW_STATIC 

INCLUDEPATH  *= ../.. $$VCGDIR
!CONFIG(system_glew): INCLUDEPATH *= $$GLEWDIR/include
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
DEFINES += _DEBUG
DEFINES += __STDC_LIMIT_MACROS

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


#DEFINES += SIMPLE_DB
#CONFIG += simpledb

#simpleb{
#SOURCES += ../ooc_vector/simpledb.cpp \
#            ../ooc_vector/io/ooc_chunks.cpp \
#            ./simpledb/ocme_simpledb.cpp \
#}
#else
#{
SOURCES += ../ooc_vector/io/ooc_chunks_kcdb.cpp\
            ./simpledb/ocme_kcdb.cpp
#}

SOURCES += main.cpp \
    ../ooc_vector/cache_policy.cpp \
    ../utils/string_serialize.cpp \
		../../../../../../vcglib/wrap/system/getopt.cpp

SOURCES += ../ooc_vector/ooc_chains.cpp
SOURCES += cell.cpp
SOURCES += ocme.cpp
SOURCES += ocme_garbage.cpp
SOURCES += ocme_debug.cpp
SOURCES += ../../../../../../vcglib/wrap/ply/plylib.cpp \
    impostor_create.cpp \
    ocme_disk_loader.cpp \
    ocme_impostor.cpp
#    plane_box_quantization.cpp

INCLUDEPATH +=  . \
    ../utils/ \
    ../ooc_vector/ \
    ../ooc_vector/io \
    ../../../../../../vcglib/ \
    /usr/include/qt4/Qt \
                ../ \
    ../../../../external/kyotocabinet-1.2.34/include

QMAKE_CXXFLAGS_RELEASE += -DNDEBUG
#LIBS += -L/usr/lib \
#    -L/usr/local/BerkeleyDB.4.7/lib \
LIBS +=    -lGLEW 
LIBS +=    -lkyotocabinet

!CONFIG(system_glew): QMAKE_LIBDIR += $$GLEWDIR/lib
QMAKE_LIBDIR += ../../../../external/kyotocabinet-1.2.34


 
