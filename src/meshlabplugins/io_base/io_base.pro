include (../../shared.pri)

HEADERS += \
    baseio.h \
    load_project.h \
    save_project.h \
    $$VCGDIR/wrap/io_trimesh/import_ply.h \
    $$VCGDIR/wrap/io_trimesh/import_obj.h \
    $$VCGDIR/wrap/io_trimesh/import_off.h \
    $$VCGDIR/wrap/io_trimesh/import_ptx.h \
    $$VCGDIR/wrap/io_trimesh/import_stl.h \
    $$VCGDIR/wrap/io_trimesh/import_fbx.h \
    $$VCGDIR/wrap/io_trimesh/export_ply.h \
    $$VCGDIR/wrap/io_trimesh/export_obj.h \
    $$VCGDIR/wrap/io_trimesh/export_off.h \
    $$VCGDIR/wrap/ply/plylib.h \
    $$VCGDIR/wrap/io_trimesh/io_material.h

SOURCES += \
    baseio.cpp \
    load_project.cpp \
    save_project.cpp \
    $$VCGDIR/wrap/ply/plylib.cpp \
    $$VCGDIR/wrap/openfbx/src/ofbx.cpp \
    $$VCGDIR/wrap/openfbx/src/miniz.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/easyexif/exif.cpp

INCLUDEPATH += $$MESHLAB_EXTERNAL_DIRECTORY/easyexif

TARGET = io_base
