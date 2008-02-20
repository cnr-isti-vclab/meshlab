TEMPLATE      = lib
CONFIG       += plugin
CONFIG += stl
INCLUDEPATH  += ../../.. ../../../../../sf ../../../../../code/lib/glew/include
HEADERS       = ../common/const_types.h \
								../common/transferfunction.h \
								../common/util.h \
								../common/meshmethods.h \
								filterqualitymapper.h \
								../../../meshlab/filterparameter.h 

SOURCES       = ../common/transferfunction.cpp \
								../common/util.cpp \
								../common/meshmethods.cpp \
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
