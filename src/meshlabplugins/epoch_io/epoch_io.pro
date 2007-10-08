TEMPLATE      = lib
CONFIG       += plugin 
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include ../../../../code/lib/bzip2-1.0.3

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
		../../../../sf/wrap/ply/plylib.cpp

                
TARGET        = epoch_io
DESTDIR       = ../../meshlab/plugins
QT           += xml
CONFIG		+= debug_and_release
# mac:CONFIG += x86 ppc

win32-msvc.net:LIBS	+= ../../../../code/lib/bzip2-1.0.3/libbz2.lib
win32-msvc2005:LIBS	+= ../../../../code/lib/bzip2-1.0.3/libbz2.lib

win32-g++:LIBS	+= ../../../../code/lib/bzip2-1.0.3/libbz2.a

# The following define is needed in gcc to remove the asserts
win32-g++:DEFINES += NDEBUG
CONFIG(debug, debug|release) {
	win32-g++:release:DEFINES -= NDEBUG
}

CONFIG(release, debug|release) {
	win32-g++:release:QMAKE_CXXFLAGS -= -O2
	win32-g++:release:QMAKE_CFLAGS -= -O2
	win32-g++:release:QMAKE_CXXFLAGS += -O3 -mtune=pentium3 -ffast-math
}	

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
mac:LIBS   += -lbz2
unix:LIBS += -lbz2


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
