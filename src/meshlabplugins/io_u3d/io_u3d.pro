include (../../shared.pri)

HEADERS += \
	io_u3d.h \
	$$VCGDIR/wrap/io_trimesh/export_u3d.h

SOURCES += \
    io_u3d.cpp

INCLUDEPATH += \
	$$MESHLAB_EXTERNAL_DIRECTORY/u3d/IDTF

win32-msvc:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc/IDTFConverter.lib
win32-g++:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc/libIDTFConverter.a
macx:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/macx64/libIDTFConverter.a
linux:LIBS += -lIDTFConverter

TARGET = io_u3d

CONFIG += STL
