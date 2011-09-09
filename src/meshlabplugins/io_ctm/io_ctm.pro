include (../../shared.pri)
HEADERS += io_ctm.h \
    $$VCGDIR/wrap/io_trimesh/import_ctm.h \
    $$VCGDIR/wrap/io_trimesh/export_ctm.h
SOURCES += io_ctm.cpp
INCLUDEPATH += ../../external/OpenCTM-1.0.3/lib
TARGET = io_ctm

macx:LIBS += ../../external/lib/macx/libopenctm.a
