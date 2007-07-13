TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = editalign.h \
                alignDialog.h \
		align/AlignPair.h \
		align/AlignGlobal.h \
                align/OccupancyGrid.h \
		 ../../../../sf/wrap/gui/trackball.h\
                 ../../../../sf/wrap/gui/trackmode.h\
                 ../../../../sf/wrap/gl/trimesh.h

SOURCES       = editalign.cpp \
                ../../meshlab/meshmodel.cpp \
		alignDialog.cpp \
                ../../../../sf/wrap/ply/plylib.cpp\
                align/AlignPair.cpp \
                align/AlignGlobal.cpp \
                align/OccupancyGrid.cpp \
		 ../../../../sf/wrap/gui/trackball.cpp\
                 ../../../../sf/wrap/gui/trackmode.cpp \
                 ../../../../code/lib/glew/src/glew.c

FORMS += alignDialog.ui
TARGET        = editalign
DESTDIR       = ../../meshlab/plugins
DEFINES += GLEW_STATIC
QT           += opengl
RESOURCES     = editalign.qrc

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

CONFIG		+= debug_and_release
#mac:CONFIG += x86 ppc

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}



