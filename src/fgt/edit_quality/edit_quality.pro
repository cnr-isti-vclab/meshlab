TEMPLATE      = lib
CONFIG       += plugin
CONFIG += stl
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = const_types.h qualitymapper.h qualitymapperdialog.h transferfunction.h ui_qualitymapperdialog.h
SOURCES       = qualitymapper.cpp qualitymapperdialog.cpp transferfunction.cpp qualitymapperdialog.cpp \
								../../meshlab/filterparameter.cpp
TARGET        = qualitymapper
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

RESOURCES = qualitymapper.qrc

FORMS		  = qualitymapperdialog.ui
CONFIG		+= debug_and_release
# mac:CONFIG += x86 ppc


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
QT           += opengl
