include (../../shared.pri)

HEADERS = \
    meshio.h \
    $$VCGDIR/wrap/ply/plylib.h \
    $$VCGDIR/wrap/io_trimesh/export_obj.h \
    $$VCGDIR/wrap/io_trimesh/export_3ds.h \
    $$VCGDIR/wrap/io_trimesh/io_material.h \
    import_3ds.h \
    io_3ds.h
				
SOURCES += \
    meshio.cpp

TARGET = io_3ds

!CONFIG(system_lib3ds) {
    INCLUDEPATH *= $$MESHLAB_EXTERNAL_DIRECTORY/lib3ds-1.3.0/
	DEFINES += LIB3DS_STATIC
}

# Notes on the paths of lib3ds files.
# Once you have compiled the library put the lib files in a dir named lib/XXX/ 
# where XXX is the name of your system according to the QT naming style.
# Linux users can rely on their own package managers and hope that it is installing the last version of lib3ds

win32-msvc:LIBS	+= $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc/3ds.lib
win32-g++:LIBS	+= $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc/lib3ds.a
macx:LIBS       += $$MESHLAB_DISTRIB_DIRECTORY/lib/macx64/lib3ds.a
linux-g++:LIBS  += -l3ds


