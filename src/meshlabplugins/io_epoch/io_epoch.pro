include (../../shared.pri)

TEMPLATE      = lib
CONFIG       += plugin 

FORMS         = ui/v3dImportDialog.ui
HEADERS       += epoch_io.h \
				epoch_reconstruction.h \
				epoch_camera.h \		
				radial_distortion.h\
				v3dImportDialog.h \
				scalar_image.h \
                maskRenderWidget.h \
                maskImageWidget.h \
                fillImage.h
				
SOURCES       += epoch_io.cpp \
                epoch_camera.cpp \
                radial_distortion.cpp \
                scalar_image.cpp \
                v3dImportDialog.cpp \ 
                maskRenderWidget.cpp \
                maskImageWidget.cpp \
                fillImage.cpp \
		$$VCGDIR/wrap/ply/plylib.cpp
                
TARGET        = io_epoch
QT           += xml

win32-msvc2005:LIBS	   += ../../external/lib/win32-msvc2005/bz2.lib
win32-msvc2008:LIBS	   += ../../external/lib/win32-msvc2008/bz2.lib
win32-g++:LIBS	+= ../../external/lib/win32-gcc/libbz2.a
linux-g++:LIBS	+= ../../external/lib/linux-g++/libbz2.a
linux-g++-32:LIBS	+= ../../external/lib/linux-g++-32/libbz2.a
linux-g++-64:LIBS	+= ../../external/lib/linux-g++-64/libbz2.a

win32-msvc2005:INCLUDEPATH += ../../external/bzip2-1.0.5
win32-msvc2008:INCLUDEPATH += ../../external/bzip2-1.0.5
win32-g++:INCLUDEPATH  += ../../external/bzip2-1.0.5
linux-g++:INCLUDEPATH	+= ../../external/bzip2-1.0.5
linux-g++-32:INCLUDEPATH	+= ../../external/bzip2-1.0.5
linux-g++-64:INCLUDEPATH	+= ../../external/bzip2-1.0.5

mac:LIBS   += -lbz2

CONFIG(release, debug|release) {
	win32-g++:release:QMAKE_CXXFLAGS -= -O2
	win32-g++:release:QMAKE_CFLAGS -= -O2
	win32-g++:release:QMAKE_CXXFLAGS += -O3 -mtune=pentium3 -ffast-math
}	

