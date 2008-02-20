TEMPLATE      = lib
CONFIG       += plugin
CONFIG += stl
INCLUDEPATH  += ../../.. ../../../../../sf ../../../../../code/lib/glew/include
HEADERS       = ../const_types.h \
								../transferfunction.h \
								../util.h \
								../qualitymapperdialog.h \
								filterqualitymapper.h \
								../../../meshlab/filterparameter.h 

SOURCES       = ../transferfunction.cpp \
								../util.cpp \
								../qualitymapperdialog.cpp \
								filterqualitymapper.cpp \
								../../../meshlab/filterparameter.cpp


TARGET        = filter_quality
DESTDIR       = ../../../meshlab/plugins

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

CONFIG		+= debug_and_release

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
QT           += opengl
