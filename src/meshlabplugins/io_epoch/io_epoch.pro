include (../../shared.pri)

TEMPLATE      = lib
CONFIG       += plugin 

FORMS         = ui/v3dImportDialog.ui
HEADERS       = epoch_io.h \
				epoch_reconstruction.h \
				epoch_camera.h \		
				radial_distortion.h\
				v3dImportDialog.h \
				scalar_image.h \
                maskRenderWidget.h \
                maskImageWidget.h \
                fillImage.h
				
SOURCES       = epoch_io.cpp \
                epoch_camera.cpp \
                radial_distortion.cpp \
                scalar_image.cpp \
                v3dImportDialog.cpp \ 
                maskRenderWidget.cpp \
                maskImageWidget.cpp \
                fillImage.cpp \
		$$VCGDIR/wrap/ply/plylib.cpp
                
TARGET        = io_epoch
DESTDIR       = ../../meshlab/plugins
QT           += xml

win32-msvc.net:LIBS 	   += ../../../../code/lib/bzip2-1.0.3/libbz2.lib
win32-msvc2005:LIBS	   += ../../../../code/lib/bzip2-1.0.4/win32/lib/bzip2.lib
win32-g++:LIBS	+= ../../../../code/lib/bzip2-1.0.3/libbz2.a

win32-msvc2005:INCLUDEPATH -= ../../../../code/lib/bzip2-1.0.3
win32-msvc2005:INCLUDEPATH += ../../../../code/lib/bzip2-1.0.4/win32/include
win32-g++:INCLUDEPATH  += ../../../../code/lib/bzip2-1.0.3

mac:LIBS   += -lbz2
unix:LIBS += -lbz2

CONFIG(release, debug|release) {
	win32-g++:release:QMAKE_CXXFLAGS -= -O2
	win32-g++:release:QMAKE_CFLAGS -= -O2
	win32-g++:release:QMAKE_CXXFLAGS += -O3 -mtune=pentium3 -ffast-math
}	

