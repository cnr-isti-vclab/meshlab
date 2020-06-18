include (../../shared.pri)

HEADERS += \
	io_u3d.h \
	$$VCGDIR/wrap/io_trimesh/export_u3d.h

SOURCES += \
	io_u3d.cpp

INCLUDEPATH += \
	$$MESHLAB_EXTERNAL_DIRECTORY/u3d/src/IDTF

win32-msvc {
	LIBS += \
		winmm.lib user32.lib \
		-L$$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc -lIDTFConverter -lIFXCoreStatic
}

win32-g++ {
	LIBS += \
		winmm.lib user32.lib \
		-L$$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc -lIDTFConverter -lIFXCoreStatic
}

macx:LIBS +=-L$$MESHLAB_DISTRIB_DIRECTORY/lib/macx64/ \
    -lIDTFConverter -lIFXCoreStatic -lIFXCore -lIFXExporting -lIFXScheduling -ldl

linux:LIBS += -lIDTFConverter -lIFXCoreStatic -ldl

TARGET = io_u3d

CONFIG += STL
